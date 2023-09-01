/*
 * NTV2 v4l2 device interface
 *
 * Copyright 2016 AJA Video Systems Inc. All rights reserved.
 *
 * This program is free software; you may redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; version 2 of the License.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
 * EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
 * MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND
 * NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS
 * BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN
 * ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN
 * CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 * SOFTWARE.
 */

#include "ntv2_serial.h"
#include "ntv2_features.h"
#include "ntv2_register.h"
#include "ntv2_konareg.h"

#define NTV2_SERIAL_CLOSE_TIMEOUT		10000000

static bool ntv2_serial_receive(struct ntv2_serial *ntv2_ser);
static bool ntv2_serial_transmit(struct ntv2_serial *ntv2_ser);
static void ntv2_serial_control(struct ntv2_serial *ntv2_ser, u32 clear_bits, u32 set_bits);

int ntv2_remove_one_port(struct uart_driver *drv, struct uart_port *uport)
{
	struct uart_state *state = drv->state + uport->line;
	struct tty_port *port = &state->port;
	struct tty_struct *tty;
	int ret = 0;

	if (state->uart_port != uport)
		dev_alert(uport->dev, "Removing wrong port: %p != %p\n",
			state->uart_port, uport);

	/*
	 * Mark the port "dead" - this prevents any opens from
	 * succeeding while we shut down the port.
	 */
	uport->flags |= UPF_DEAD;

	/*
	 * Remove the devices from the tty layer
	 */
	tty_unregister_device(drv->tty_driver, uport->line);

	tty = tty_port_tty_get(port);
	if (tty) {
		tty_vhangup(port->tty);
		tty_kref_put(tty);
	}

	/*
	 * Hack to wait for tty to close to prevent panic
	 */
	msleep(250);

	/*
	 * Free the port IO and memory resources, if any.
	 */
	if (uport->type != PORT_UNKNOWN)
		uport->ops->release_port(uport);

#ifdef NTV2_USE_TTY_GROUP
	kfree(uport->tty_groups);
#endif

	/*
	 * Indicate that there isn't a port here anymore.
	 */
	uport->type = PORT_UNKNOWN;

	state->uart_port = NULL;

	return ret;
}

static unsigned int ntv2_uartops_tx_empty(struct uart_port *port)
{
	struct ntv2_serial *ntv2_ser = container_of(port, struct ntv2_serial, uart_port);
	u32 empty = NTV2_FLD_MASK(ntv2_kona_fld_serial_tx_empty);
	unsigned long flags;
	unsigned int ret;

	/* empty if not enabled */
	if (!ntv2_ser->uart_enable)
		return TIOCSER_TEMT;

	spin_lock_irqsave(&port->lock, flags);
	ret = ntv2_reg_read(ntv2_ser->vid_reg, ntv2_kona_reg_serial_status, ntv2_ser->index);
	spin_unlock_irqrestore(&port->lock, flags);

	return (ret & empty)? TIOCSER_TEMT : 0;
}

static unsigned int ntv2_uartops_get_mctrl(struct uart_port *port)
{
	return TIOCM_CTS | TIOCM_DSR | TIOCM_CAR;
}

static void ntv2_uartops_set_mctrl(struct uart_port *port, unsigned int mctrl)
{
	/* N/A */
}

static void ntv2_uartops_stop_tx(struct uart_port *port)
{
	struct ntv2_serial *ntv2_ser = container_of(port, struct ntv2_serial, uart_port);

	NTV2_MSG_SERIAL_STREAM("%s: uart stop transmit\n", ntv2_ser->name);
}

static void ntv2_uartops_start_tx(struct uart_port *port)
{
	struct ntv2_serial *ntv2_ser = container_of(port, struct ntv2_serial, uart_port);

	NTV2_MSG_SERIAL_STREAM("%s: uart start transmit\n", ntv2_ser->name);

	ntv2_serial_transmit(ntv2_ser);
}

static void ntv2_uartops_stop_rx(struct uart_port *port)
{
	struct ntv2_serial *ntv2_ser = container_of(port, struct ntv2_serial, uart_port);

	NTV2_MSG_SERIAL_STREAM("%s: uart stop receive\n", ntv2_ser->name);

	/* don't forward any more data (like !CREAD) */
	port->ignore_status_mask = 
		NTV2_FLD_MASK(ntv2_kona_fld_serial_rx_valid) |
		NTV2_FLD_MASK(ntv2_kona_fld_serial_error_overrun) |
		NTV2_FLD_MASK(ntv2_kona_fld_serial_error_frame) |
		NTV2_FLD_MASK(ntv2_kona_fld_serial_error_parity);
}

static void ntv2_uartops_break_ctl(struct uart_port *port, int ctl)
{
	/* N/A */
}

static int ntv2_uartops_startup(struct uart_port *port)
{
	struct ntv2_serial *ntv2_ser = container_of(port, struct ntv2_serial, uart_port);
	int ret;

	NTV2_MSG_SERIAL_STREAM("%s: uart startup\n", ntv2_ser->name);

	/* enable serial port */
	ret = ntv2_serial_enable(ntv2_ser);

	return ret;
}

static void ntv2_uartops_shutdown(struct uart_port *port)
{
	struct ntv2_serial *ntv2_ser = container_of(port, struct ntv2_serial, uart_port);

	NTV2_MSG_SERIAL_STREAM("%s: uart shutdown\n", ntv2_ser->name);

	ntv2_serial_disable(ntv2_ser);
}

#ifdef NTV2_USE_TERMIOS_CONST
static void ntv2_uartops_set_termios(struct uart_port *port,
									 struct ktermios *termios,
									 const struct ktermios *old)

#else
static void ntv2_uartops_set_termios(struct uart_port *port,
									 struct ktermios *termios,
									 struct ktermios *old)
#endif
{
	struct ntv2_serial *ntv2_ser = container_of(port, struct ntv2_serial, uart_port);
	u32 valid = NTV2_FLD_MASK(ntv2_kona_fld_serial_rx_valid);
	u32 overrun = NTV2_FLD_MASK(ntv2_kona_fld_serial_error_overrun);
	u32 frame = NTV2_FLD_MASK(ntv2_kona_fld_serial_error_frame);
	u32 parity = NTV2_FLD_MASK(ntv2_kona_fld_serial_error_parity);
	u32 full = NTV2_FLD_MASK(ntv2_kona_fld_serial_tx_full);
	unsigned long flags;
	unsigned int baud;

	NTV2_MSG_SERIAL_STREAM("%s: uart set termios\n", ntv2_ser->name);

	spin_lock_irqsave(&port->lock, flags);

	port->read_status_mask = valid | overrun | full;

	if (termios->c_iflag & INPCK)
		port->read_status_mask |= parity | frame;

	port->ignore_status_mask = 0;
	if (termios->c_iflag & IGNPAR)
		port->ignore_status_mask |= parity | frame | overrun;

	/* ignore all characters if CREAD is not set */
	if ((termios->c_cflag & CREAD) == 0)
		port->ignore_status_mask |= valid | parity | frame | overrun;

	/* update timeout */
	baud = uart_get_baud_rate(port, termios, old, 0, 460800);
	uart_update_timeout(port, termios->c_cflag, baud);

	spin_unlock_irqrestore(&port->lock, flags);
}

static const char *ntv2_uartops_type(struct uart_port *port)
{
	return port->type == PORT_UARTLITE ? "uartlite" : NULL;
}

static void ntv2_uartops_release_port(struct uart_port *port)
{
	struct ntv2_serial *ntv2_ser = container_of(port, struct ntv2_serial, uart_port);

	NTV2_MSG_SERIAL_STREAM("%s: uart release port\n", ntv2_ser->name);

	ntv2_ser->busy = false;
}

static int ntv2_uartops_request_port(struct uart_port *port)
{
	struct ntv2_serial *ntv2_ser = container_of(port, struct ntv2_serial, uart_port);

	NTV2_MSG_SERIAL_STREAM("%s: uart request port\n", ntv2_ser->name);

	/* try to allocate uart */
	if (ntv2_ser->busy)
		return -EBUSY;
	ntv2_ser->busy = true;

	return 0;
}

static void ntv2_uartops_config_port(struct uart_port *port, int flags)
{
	struct ntv2_serial *ntv2_ser = container_of(port, struct ntv2_serial, uart_port);

	NTV2_MSG_SERIAL_STREAM("%s: uart config port\n", ntv2_ser->name);

//	if (!ntv2_uartops_request_port(port))
	if (flags & UART_CONFIG_TYPE) {
		port->type = PORT_UARTLITE;
		ntv2_uartops_request_port(port);
	}
}

static int ntv2_uartops_verify_port(struct uart_port *port, struct serial_struct *ser)
{
	struct ntv2_serial *ntv2_ser = container_of(port, struct ntv2_serial, uart_port);

	NTV2_MSG_SERIAL_STREAM("%s: uart verify port\n", ntv2_ser->name);

	return -EINVAL;
}

static struct uart_ops ntv2_uartops = {
	.tx_empty		= ntv2_uartops_tx_empty,
	.set_mctrl		= ntv2_uartops_set_mctrl,
	.get_mctrl		= ntv2_uartops_get_mctrl,
	.stop_tx		= ntv2_uartops_stop_tx,
	.start_tx		= ntv2_uartops_start_tx,
	.stop_rx		= ntv2_uartops_stop_rx,
	.break_ctl		= ntv2_uartops_break_ctl,
	.startup		= ntv2_uartops_startup,
	.shutdown		= ntv2_uartops_shutdown,
	.set_termios	= ntv2_uartops_set_termios,
	.type			= ntv2_uartops_type,
	.release_port	= ntv2_uartops_release_port,
	.request_port	= ntv2_uartops_request_port,
	.config_port	= ntv2_uartops_config_port,
	.verify_port	= ntv2_uartops_verify_port,
};

struct ntv2_serial *ntv2_serial_open(struct ntv2_object *ntv2_obj,
								   const char *name, int index)
{
	struct ntv2_serial *ntv2_ser = NULL;

	if (ntv2_obj == NULL)
		return NULL;

	ntv2_ser = kzalloc(sizeof(struct ntv2_serial), GFP_KERNEL);
	if (ntv2_ser == NULL) {
		NTV2_MSG_ERROR("%s: ntv2_serial instance memory allocation failed\n", ntv2_obj->name);
		return NULL;
	}

	ntv2_ser->index = index;
	snprintf(ntv2_ser->name, NTV2_STRING_SIZE, "%s-%s%d", ntv2_obj->name, name, index);
	INIT_LIST_HEAD(&ntv2_ser->list);
	ntv2_ser->ntv2_dev = ntv2_obj->ntv2_dev;

	spin_lock_init(&ntv2_ser->state_lock);

	NTV2_MSG_SERIAL_INFO("%s: open ntv2_serial\n", ntv2_ser->name);

	return ntv2_ser;
}

void ntv2_serial_close(struct ntv2_serial *ntv2_ser)
{
	struct uart_port *port;

	if (ntv2_ser == NULL)
		return;

	NTV2_MSG_SERIAL_INFO("%s: close ntv2_serial\n", ntv2_ser->name);

	/* stop the uart */
	ntv2_serial_disable(ntv2_ser);

	port = &ntv2_ser->uart_port;
	if (port->iobase != 0) {
		ntv2_remove_one_port(ntv2_module_info()->uart_driver, port);
		port->iobase = 0;
	}

	memset(ntv2_ser, 0, sizeof(struct ntv2_serial));
	kfree(ntv2_ser);
}

int ntv2_serial_configure(struct ntv2_serial *ntv2_ser,
						  struct ntv2_features *features,
						  struct ntv2_register *vid_reg)
{
	struct ntv2_module *ntv2_mod = ntv2_module_info();
	struct uart_port *port;
	int index;
	int ret;

	if ((ntv2_ser == NULL) ||
		(features == NULL) ||
		(vid_reg == NULL))
		return -EPERM;

	NTV2_MSG_SERIAL_INFO("%s: configure serial device\n", ntv2_ser->name);

	ntv2_ser->features = features;
	ntv2_ser->vid_reg = vid_reg;

	/* get next serial port index */
	index = atomic_inc_return(&ntv2_mod->uart_index) - 1;
	if (index >= ntv2_mod->uart_max) {
		NTV2_MSG_SERIAL_ERROR("%s: ntv2_serial too many uarts %d\n", ntv2_ser->name, index + 1);
		return -ENOMEM;
	}

	/* configure the serial port */
	port = &ntv2_ser->uart_port;
	port->fifosize = features->serial_config[ntv2_ser->index]->fifo_size;
	port->regshift = 2;
	port->iotype = UPIO_MEM;
	port->iobase = 1; /* mark port in use */
	port->ops = &ntv2_uartops;
	port->flags = UPF_BOOT_AUTOCONF;
	port->dev = &ntv2_ser->ntv2_dev->pci_dev->dev;
	port->type = PORT_UNKNOWN;
	port->line = (unsigned int)index;

	NTV2_MSG_SERIAL_INFO("%s: register serial device: %s  port: %d\n",
						 ntv2_ser->name, NTV2_TTY_NAME, index);

	/* register the serial port */
	ret = uart_add_one_port(ntv2_mod->uart_driver, port);
	if (ret < 0) {
		NTV2_MSG_SERIAL_ERROR("%s: uart_add_one_port() failed %d  port: %d\n",
							  ntv2_ser->name, ret, index);
		port->iobase = 0;
		return ret;
	}

	return 0;
}

int ntv2_serial_enable(struct ntv2_serial *ntv2_ser)
{
	u32 reset_tx = NTV2_FLD_MASK(ntv2_kona_fld_serial_reset_tx);
	u32 reset_rx = NTV2_FLD_MASK(ntv2_kona_fld_serial_reset_rx);
	u32 enable = NTV2_FLD_MASK(ntv2_kona_fld_serial_interrupt_enable);
	unsigned long flags;

	if (ntv2_ser == NULL)
		return -EPERM;

	spin_lock_irqsave(&ntv2_ser->state_lock, flags);

	if (ntv2_ser->uart_enable) {
		spin_unlock_irqrestore(&ntv2_ser->state_lock, flags);
		return 0;
	}

	NTV2_MSG_SERIAL_STATE("%s: serial state: enable\n", ntv2_ser->name);

	ntv2_ser->uart_enable = true;

	/* enable interrupt */
	ntv2_serial_control(ntv2_ser, 0, reset_tx | reset_rx | enable);

	spin_unlock_irqrestore(&ntv2_ser->state_lock, flags);

	return 0;
}

int ntv2_serial_disable(struct ntv2_serial *ntv2_ser)
{
	u32 enable = NTV2_FLD_MASK(ntv2_kona_fld_serial_interrupt_enable);
	unsigned long flags;

	if (ntv2_ser == NULL)
		return -EPERM;

	spin_lock_irqsave(&ntv2_ser->state_lock, flags);
	
	if (!ntv2_ser->uart_enable) {
		spin_unlock_irqrestore(&ntv2_ser->state_lock, flags);
		return 0;
	}

	NTV2_MSG_SERIAL_STATE("%s: serial state: disable\n", ntv2_ser->name);

	ntv2_ser->uart_enable = false;

	/* disable interrupt */
	ntv2_serial_control(ntv2_ser, enable, 0);

	spin_unlock_irqrestore(&ntv2_ser->state_lock, flags);

	return 0;
}

int ntv2_serial_interrupt(struct ntv2_serial *ntv2_ser,
						  struct ntv2_interrupt_status* irq_status)
{
	struct uart_port *port = &ntv2_ser->uart_port;
	u32 active = NTV2_FLD_MASK(ntv2_kona_fld_serial_int_active);
	u32 clear = NTV2_FLD_MASK(ntv2_kona_fld_serial_interrupt_clear);
	u32 status;
	bool busy;
	int count = 0;
	unsigned long flags;

	if ((ntv2_ser == NULL) ||
		(irq_status == NULL))
		return IRQ_NONE;

	spin_lock_irqsave(&ntv2_ser->state_lock, flags);

	/* is interrupt active */
	status = ntv2_reg_read(ntv2_ser->vid_reg, ntv2_kona_reg_serial_status, ntv2_ser->index);
	if ((status & active) == 0) {
		spin_unlock_irqrestore(&ntv2_ser->state_lock, flags);
		return IRQ_NONE;
	}

	/* clear interrupt */
	ntv2_serial_control(ntv2_ser, 0, clear);

	/* check enabled */
	if (!ntv2_ser->uart_enable) {
		spin_unlock_irqrestore(&ntv2_ser->state_lock, flags);
		return IRQ_HANDLED;
	}

//	NTV2_MSG_SERIAL_STREAM("%s: uart interrupt status %02x\n", ntv2_ser->name, status);

	/* manage uart */
	do {
		busy  = ntv2_serial_receive(ntv2_ser);
		busy |= ntv2_serial_transmit(ntv2_ser);
		count++;
	} while (busy);

	if (count > 1)
		tty_flip_buffer_push(&port->state->port);

	spin_unlock_irqrestore(&ntv2_ser->state_lock, flags);

	return IRQ_HANDLED;
}

static bool ntv2_serial_receive(struct ntv2_serial *ntv2_ser)
{
	struct uart_port *port = &ntv2_ser->uart_port;
	struct tty_port *tport = &port->state->port;
	u32 valid = NTV2_FLD_MASK(ntv2_kona_fld_serial_rx_valid);
	u32 overrun = NTV2_FLD_MASK(ntv2_kona_fld_serial_error_overrun);
	u32 frame = NTV2_FLD_MASK(ntv2_kona_fld_serial_error_frame);
	u32 parity = NTV2_FLD_MASK(ntv2_kona_fld_serial_error_parity);
	u32 trigger = NTV2_FLD_MASK(ntv2_kona_fld_serial_rx_trigger);
	u32 active = NTV2_FLD_MASK(ntv2_kona_fld_serial_rx_active);
	u32 status;
	u32 rx = 0;
	int i;

	char flag = TTY_NORMAL;

	status = ntv2_reg_read(ntv2_ser->vid_reg, ntv2_kona_reg_serial_status, ntv2_ser->index);
	if ((status & (valid | overrun | frame)) == 0)
		return false;

	/* gather statistics */
	if ((status & valid) != 0) {
		port->icount.rx++;

		/* trigger read of uart rx fifo */
		ntv2_serial_control(ntv2_ser, 0, trigger);

		/* read rx data from pci */
		for (i = 0; i < 10; i++) {
			rx = ntv2_reg_read(ntv2_ser->vid_reg, ntv2_kona_reg_serial_rx, ntv2_ser->index);
			if ((rx & active) == 0)
				break;
		}
			
		NTV2_MSG_SERIAL_STREAM("%s: uart rx %02x  busy %d\n", ntv2_ser->name, (u8)rx, i);

		if ((status & parity) != 0)
			port->icount.parity++;
	}

	if ((status & overrun) != 0)
		port->icount.overrun++;

	if ((status & frame) != 0)
		port->icount.frame++;

	/* drop byte with parity error if IGNPAR specificed */
	if ((status & port->ignore_status_mask & parity) != 0)
		status &= ~valid;

	status &= port->read_status_mask;

	if ((status & parity) != 0)
		flag = TTY_PARITY;

	status &= ~port->ignore_status_mask;

	if ((status & valid) != 0)
		tty_insert_flip_char(tport, (u8)rx, flag);

	if ((status & overrun) != 0)
		tty_insert_flip_char(tport, 0, TTY_OVERRUN);

	if ((status & frame) != 0)
		tty_insert_flip_char(tport, 0, TTY_FRAME);

	return true;
}

static bool ntv2_serial_transmit(struct ntv2_serial *ntv2_ser)
{
	struct uart_port *port = &ntv2_ser->uart_port;
	struct circ_buf *xmit  = &port->state->xmit;
	u32 full = NTV2_FLD_MASK(ntv2_kona_fld_serial_tx_full);
	u32 status;

	status = ntv2_reg_read(ntv2_ser->vid_reg, ntv2_kona_reg_serial_status, ntv2_ser->index);
	if (status & full)
		return false;

	/* tx xon/xoff */
	if ((port->x_char) != 0) {
		NTV2_MSG_SERIAL_STREAM("%s: uart tx %02x\n", ntv2_ser->name, (u8)port->x_char);
		ntv2_reg_write(ntv2_ser->vid_reg,
					   ntv2_kona_reg_serial_tx, ntv2_ser->index,
					   (u32)port->x_char);
		port->x_char = 0;
		port->icount.tx++;
		return true;
	}

	if (uart_circ_empty(xmit) || uart_tx_stopped(port))
		return false;

	/* tx data */
	NTV2_MSG_SERIAL_STREAM("%s: uart tx %02x\n", ntv2_ser->name, (u8)xmit->buf[xmit->tail]);
	ntv2_reg_write(ntv2_ser->vid_reg,
				   ntv2_kona_reg_serial_tx, ntv2_ser->index,
				   (u32)xmit->buf[xmit->tail]);
	xmit->tail = (xmit->tail + 1) & (UART_XMIT_SIZE-1);
	port->icount.tx++;

	/* wake up */
	if (uart_circ_chars_pending(xmit) < WAKEUP_CHARS)
		uart_write_wakeup(port);

	return true;
}

static void ntv2_serial_control(struct ntv2_serial *ntv2_ser, u32 clear_bits, u32 set_bits)
{
	u32 enable = NTV2_FLD_MASK(ntv2_kona_fld_serial_interrupt_enable);
	u32 loop = NTV2_FLD_MASK(ntv2_kona_fld_serial_loopback_enable);
	u32 status;

	if (ntv2_ser == NULL)
		return;

	/* read current status */
	status = ntv2_reg_read(ntv2_ser->vid_reg, ntv2_kona_reg_serial_status, ntv2_ser->index);

	/* filter non state bits */
	status &= enable | loop;

	/* clear and set bits */
	status = (status & ~clear_bits) | set_bits;

	/* write control */
	ntv2_reg_write(ntv2_ser->vid_reg, ntv2_kona_reg_serial_control, ntv2_ser->index, status);
}
