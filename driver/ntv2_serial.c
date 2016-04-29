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
	spin_lock_init(&ntv2_ser->int_lock);

	NTV2_MSG_SERIAL_INFO("%s: open ntv2_serial\n", ntv2_ser->name);

	return ntv2_ser;
}

void ntv2_serial_close(struct ntv2_serial *ntv2_ser)
{
	if (ntv2_ser == NULL)
		return;

	NTV2_MSG_SERIAL_INFO("%s: close ntv2_serial\n", ntv2_ser->name);

	/* stop the uart */
	ntv2_serial_disable(ntv2_ser);

	memset(ntv2_ser, 0, sizeof(struct ntv2_serial));
	kfree(ntv2_ser);
}

int ntv2_serial_configure(struct ntv2_serial *ntv2_ser,
						  struct ntv2_features *features,
						  struct ntv2_register *vid_reg)
{
	if ((ntv2_ser == NULL) ||
		(features == NULL) ||
		(vid_reg == NULL))
		return -EPERM;

	NTV2_MSG_SERIAL_INFO("%s: configure serial device\n", ntv2_ser->name);

	ntv2_ser->features = features;
	ntv2_ser->vid_reg = vid_reg;

//	NTV2_MSG_SERIAL_INFO("%s: register serial device: %s\n",
//						 ntv2_ser->name, serial_dev->name);

	return 0;
}

int ntv2_serial_enable(struct ntv2_serial *ntv2_ser)
{
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

	spin_unlock_irqrestore(&ntv2_ser->state_lock, flags);

	return 0;
}

int ntv2_serial_disable(struct ntv2_serial *ntv2_ser)
{
	unsigned long flags;

	if (ntv2_ser == NULL)
		return -EPERM;


	spin_lock_irqsave(&ntv2_ser->state_lock, flags);
	
	if (!ntv2_ser->uart_enable) {
		spin_unlock_irqrestore(&ntv2_ser->state_lock, flags);
		return 0;
	}

	NTV2_MSG_CHANNEL_STATE("%s: serial state: disable\n", ntv2_ser->name);

	ntv2_ser->uart_enable = false;

	spin_unlock_irqrestore(&ntv2_ser->state_lock, flags);

	return 0;
}

int ntv2_serial_interrupt(struct ntv2_serial *ntv2_ser,
						  struct ntv2_interrupt_status* irq_status)
{
	int index;
//	int res = IRQ_NONE;
	unsigned long flags;

	if ((ntv2_ser == NULL) ||
		(irq_status == NULL))
		return IRQ_NONE;

	index = ntv2_ser->index;

	spin_lock_irqsave(&ntv2_ser->int_lock, flags);
	spin_unlock_irqrestore(&ntv2_ser->int_lock, flags);

	return IRQ_HANDLED;
}

#define ULITE_NAME		"ttyUL"
#define ULITE_MAJOR		204
#define ULITE_MINOR		187
#define ULITE_NR_UARTS		4

#define ULITE_RX				0x00
#define ULITE_TX				0x04
#define ULITE_STATUS			0x08
#define ULITE_CONTROL			0x0c

#define ULITE_REGION			16

#define ULITE_STATUS_RXVALID	0x01
#define ULITE_STATUS_RXFULL		0x02
#define ULITE_STATUS_TXEMPTY	0x04
#define ULITE_STATUS_TXFULL		0x08
#define ULITE_STATUS_IE			0x10
#define ULITE_STATUS_OVERRUN	0x20
#define ULITE_STATUS_FRAME		0x40
#define ULITE_STATUS_PARITY		0x80

#define ULITE_CONTROL_RST_TX	0x01
#define ULITE_CONTROL_RST_RX	0x02
#define ULITE_CONTROL_IE		0x10

static inline u32 uart_in32(u32 offset, struct uart_port *port)
{
#if 0
	struct uartlite_reg_ops *reg_ops = port->private_data;

	return reg_ops->in(port->membase + offset);
#endif
	return 0;
}

static inline void uart_out32(u32 val, u32 offset, struct uart_port *port)
{
#if 0
	struct uartlite_reg_ops *reg_ops = port->private_data;

	reg_ops->out(val, port->membase + offset);
#endif
}

static int ulite_receive(struct uart_port *port, int stat)
{
	struct tty_port *tport = &port->state->port;
	unsigned char ch = 0;
	char flag = TTY_NORMAL;

	if ((stat & (ULITE_STATUS_RXVALID | ULITE_STATUS_OVERRUN
		     | ULITE_STATUS_FRAME)) == 0)
		return 0;

	/* stats */
	if (stat & ULITE_STATUS_RXVALID) {
		port->icount.rx++;
		ch = uart_in32(ULITE_RX, port);

		if (stat & ULITE_STATUS_PARITY)
			port->icount.parity++;
	}

	if (stat & ULITE_STATUS_OVERRUN)
		port->icount.overrun++;

	if (stat & ULITE_STATUS_FRAME)
		port->icount.frame++;


	/* drop byte with parity error if IGNPAR specificed */
	if (stat & port->ignore_status_mask & ULITE_STATUS_PARITY)
		stat &= ~ULITE_STATUS_RXVALID;

	stat &= port->read_status_mask;

	if (stat & ULITE_STATUS_PARITY)
		flag = TTY_PARITY;

	stat &= ~port->ignore_status_mask;

	if (stat & ULITE_STATUS_RXVALID)
		tty_insert_flip_char(tport, ch, flag);

	if (stat & ULITE_STATUS_FRAME)
		tty_insert_flip_char(tport, 0, TTY_FRAME);

	if (stat & ULITE_STATUS_OVERRUN)
		tty_insert_flip_char(tport, 0, TTY_OVERRUN);

	return 1;
}

static int ulite_transmit(struct uart_port *port, int stat)
{
	struct circ_buf *xmit  = &port->state->xmit;

	if (stat & ULITE_STATUS_TXFULL)
		return 0;

	if (port->x_char) {
		uart_out32(port->x_char, ULITE_TX, port);
		port->x_char = 0;
		port->icount.tx++;
		return 1;
	}

	if (uart_circ_empty(xmit) || uart_tx_stopped(port))
		return 0;

	uart_out32(xmit->buf[xmit->tail], ULITE_TX, port);
	xmit->tail = (xmit->tail + 1) & (UART_XMIT_SIZE-1);
	port->icount.tx++;

	/* wake up */
	if (uart_circ_chars_pending(xmit) < WAKEUP_CHARS)
		uart_write_wakeup(port);

	return 1;
}

static irqreturn_t ulite_isr(int irq, void *dev_id)
{
	struct uart_port *port = dev_id;
	int busy, n = 0;

	do {
		int stat = uart_in32(ULITE_STATUS, port);
		busy  = ulite_receive(port, stat);
		busy |= ulite_transmit(port, stat);
		n++;
	} while (busy);

	/* work done? */
	if (n > 1) {
		tty_flip_buffer_push(&port->state->port);
		return IRQ_HANDLED;
	} else {
		return IRQ_NONE;
	}
}

static unsigned int ulite_tx_empty(struct uart_port *port)
{
	unsigned long flags;
	unsigned int ret;

	spin_lock_irqsave(&port->lock, flags);
	ret = uart_in32(ULITE_STATUS, port);
	spin_unlock_irqrestore(&port->lock, flags);

	return ret & ULITE_STATUS_TXEMPTY ? TIOCSER_TEMT : 0;
}

static unsigned int ulite_get_mctrl(struct uart_port *port)
{
	return TIOCM_CTS | TIOCM_DSR | TIOCM_CAR;
}

static void ulite_set_mctrl(struct uart_port *port, unsigned int mctrl)
{
	/* N/A */
}

static void ulite_stop_tx(struct uart_port *port)
{
	/* N/A */
}

static void ulite_start_tx(struct uart_port *port)
{
	ulite_transmit(port, uart_in32(ULITE_STATUS, port));
}

static void ulite_stop_rx(struct uart_port *port)
{
	/* don't forward any more data (like !CREAD) */
	port->ignore_status_mask = ULITE_STATUS_RXVALID | ULITE_STATUS_PARITY
		| ULITE_STATUS_FRAME | ULITE_STATUS_OVERRUN;
}

static void ulite_break_ctl(struct uart_port *port, int ctl)
{
	/* N/A */
}

static int ulite_startup(struct uart_port *port)
{
#if 0
	int ret;

	ret = request_irq(port->irq, ulite_isr, IRQF_SHARED, "uartlite", port);
	if (ret)
		return ret;
#endif
	uart_out32(ULITE_CONTROL_RST_RX | ULITE_CONTROL_RST_TX,
		ULITE_CONTROL, port);
	uart_out32(ULITE_CONTROL_IE, ULITE_CONTROL, port);

	return 0;
}

static void ulite_shutdown(struct uart_port *port)
{
	uart_out32(0, ULITE_CONTROL, port);
	uart_in32(ULITE_CONTROL, port); /* dummy */
#if 0
	free_irq(port->irq, port);
#endif
}

static void ulite_set_termios(struct uart_port *port, struct ktermios *termios,
			      struct ktermios *old)
{
	unsigned long flags;
	unsigned int baud;

	spin_lock_irqsave(&port->lock, flags);

	port->read_status_mask = ULITE_STATUS_RXVALID | ULITE_STATUS_OVERRUN
		| ULITE_STATUS_TXFULL;

	if (termios->c_iflag & INPCK)
		port->read_status_mask |=
			ULITE_STATUS_PARITY | ULITE_STATUS_FRAME;

	port->ignore_status_mask = 0;
	if (termios->c_iflag & IGNPAR)
		port->ignore_status_mask |= ULITE_STATUS_PARITY
			| ULITE_STATUS_FRAME | ULITE_STATUS_OVERRUN;

	/* ignore all characters if CREAD is not set */
	if ((termios->c_cflag & CREAD) == 0)
		port->ignore_status_mask |=
			ULITE_STATUS_RXVALID | ULITE_STATUS_PARITY
			| ULITE_STATUS_FRAME | ULITE_STATUS_OVERRUN;

	/* update timeout */
	baud = uart_get_baud_rate(port, termios, old, 0, 460800);
	uart_update_timeout(port, termios->c_cflag, baud);

	spin_unlock_irqrestore(&port->lock, flags);
}

static const char *ulite_type(struct uart_port *port)
{
	return port->type == PORT_UARTLITE ? "uartlite" : NULL;
}

static void ulite_release_port(struct uart_port *port)
{
#if 0
	release_mem_region(port->mapbase, ULITE_REGION);
	iounmap(port->membase);
	port->membase = NULL;
#endif
}

static int ulite_request_port(struct uart_port *port)
{
#if 0
	int ret;

	pr_debug("ulite console: port=%p; port->mapbase=%llx\n",
		 port, (unsigned long long) port->mapbase);

	if (!request_mem_region(port->mapbase, ULITE_REGION, "uartlite")) {
		dev_err(port->dev, "Memory region busy\n");
		return -EBUSY;
	}

	port->membase = ioremap(port->mapbase, ULITE_REGION);
	if (!port->membase) {
		dev_err(port->dev, "Unable to map registers\n");
		release_mem_region(port->mapbase, ULITE_REGION);
		return -EBUSY;
	}

	port->private_data = &uartlite_be;
	ret = uart_in32(ULITE_CONTROL, port);
	uart_out32(ULITE_CONTROL_RST_TX, ULITE_CONTROL, port);
	ret = uart_in32(ULITE_STATUS, port);
	/* Endianess detection */
	if ((ret & ULITE_STATUS_TXEMPTY) != ULITE_STATUS_TXEMPTY)
		port->private_data = &uartlite_le;
#endif
	return 0;
}

static void ulite_config_port(struct uart_port *port, int flags)
{
	if (!ulite_request_port(port))
		port->type = PORT_UARTLITE;
}

static int ulite_verify_port(struct uart_port *port, struct serial_struct *ser)
{
	/* we don't want the core code to modify any port params */
	return -EINVAL;
}

static struct uart_ops ulite_ops = {
	.tx_empty		= ulite_tx_empty,
	.set_mctrl		= ulite_set_mctrl,
	.get_mctrl		= ulite_get_mctrl,
	.stop_tx		= ulite_stop_tx,
	.start_tx		= ulite_start_tx,
	.stop_rx		= ulite_stop_rx,
	.break_ctl		= ulite_break_ctl,
	.startup		= ulite_startup,
	.shutdown		= ulite_shutdown,
	.set_termios	= ulite_set_termios,
	.type			= ulite_type,
	.release_port	= ulite_release_port,
	.request_port	= ulite_request_port,
	.config_port	= ulite_config_port,
	.verify_port	= ulite_verify_port,
};

static struct uart_driver ulite_uart_driver;

#if 0
/* ---------------------------------------------------------------------
 * Port assignment functions (mapping devices to uart_port structures)
 */

/** ulite_assign: register a uartlite device with the driver
 *
 * @dev: pointer to device structure
 * @id: requested id number.  Pass -1 for automatic port assignment
 * @base: base address of uartlite registers
 * @irq: irq number for uartlite
 *
 * Returns: 0 on success, <0 otherwise
 */
static int ulite_assign(struct device *dev, int id, u32 base, int irq)
{
	struct uart_port *port;
	int rc;

	/* if id = -1; then scan for a free id and use that */
	if (id < 0) {
		for (id = 0; id < ULITE_NR_UARTS; id++)
			if (ulite_ports[id].mapbase == 0)
				break;
	}
	if (id < 0 || id >= ULITE_NR_UARTS) {
		dev_err(dev, "%s%i too large\n", ULITE_NAME, id);
		return -EINVAL;
	}

	if ((ulite_ports[id].mapbase) && (ulite_ports[id].mapbase != base)) {
		dev_err(dev, "cannot assign to %s%i; it is already in use\n",
			ULITE_NAME, id);
		return -EBUSY;
	}

	port = &ulite_ports[id];

	spin_lock_init(&port->lock);
	port->fifosize = 16;
	port->regshift = 2;
	port->iotype = UPIO_MEM;
	port->iobase = 1; /* mark port in use */
	port->mapbase = base;
	port->membase = NULL;
	port->ops = &ulite_ops;
	port->irq = irq;
	port->flags = UPF_BOOT_AUTOCONF;
	port->dev = dev;
	port->type = PORT_UNKNOWN;
	port->line = id;

	dev_set_drvdata(dev, port);

	/* Register the port */
	rc = uart_add_one_port(&ulite_uart_driver, port);
	if (rc) {
		dev_err(dev, "uart_add_one_port() failed; err=%i\n", rc);
		port->mapbase = 0;
		dev_set_drvdata(dev, NULL);
		return rc;
	}

	return 0;
}

/** ulite_release: register a uartlite device with the driver
 *
 * @dev: pointer to device structure
 */
static int ulite_release(struct device *dev)
{
	struct uart_port *port = dev_get_drvdata(dev);
	int rc = 0;

	if (port) {
		rc = uart_remove_one_port(&ulite_uart_driver, port);
		dev_set_drvdata(dev, NULL);
		port->mapbase = 0;
	}

	return rc;
}


static int __init ulite_init(void)
{
	int ret;

	pr_debug("uartlite: calling uart_register_driver()\n");
	ret = uart_register_driver(&ulite_uart_driver);
	if (ret)
		goto err_uart;

	pr_debug("uartlite: calling platform_driver_register()\n");
	ret = platform_driver_register(&ulite_platform_driver);
	if (ret)
		goto err_plat;

	return 0;

err_plat:
	uart_unregister_driver(&ulite_uart_driver);
err_uart:
	pr_err("registering uartlite driver failed: err=%i", ret);
	return ret;
}

static void __exit ulite_exit(void)
{
	platform_driver_unregister(&ulite_platform_driver);
	uart_unregister_driver(&ulite_uart_driver);
}

#endif
