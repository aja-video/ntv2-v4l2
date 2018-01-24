/*
 * AJA NTV2 video for linux driver
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

#include "ntv2_common.h"
#include "ntv2_device.h"
#include "ntv2_nwldma.h"

MODULE_DESCRIPTION("AJA NTV2 V4L2 Driver");
MODULE_AUTHOR("AJA Video Systems Inc. (http://www.aja.com)");
MODULE_LICENSE("GPL v2");


static int ntv2_probe(struct pci_dev *pdev, const struct pci_device_id *ent)
{
	struct ntv2_module *ntv2_mod = ntv2_module_info();
	struct ntv2_device *ntv2_dev = NULL;
	unsigned long flags;
	int index;
	int result;

	/* enable device */
	result = pci_enable_device(pdev);
	if (result != 0) {
		NTV2_MSG_ERROR("%s: pci_enable_device failed code %d\n",
					   NTV2_MODULE_NAME, result);
		return result;
	}

	/* allocate and initialize ntv2 device instance */
	index = atomic_inc_return(&ntv2_mod->device_index) - 1;
	ntv2_dev = ntv2_device_open(ntv2_mod, "dev", index);

	NTV2_MSG_INFO("%s: device probe start\n", ntv2_dev->name);

	/* set drvdata */
	dev_set_drvdata(&pdev->dev, ntv2_dev);

	result = ntv2_device_configure(ntv2_dev, pdev);
	if (result != 0) 
		goto disable_device;

	/* add to the device list */
	spin_lock_irqsave(&ntv2_mod->device_lock, flags);
	list_add_tail(&ntv2_dev->list, &ntv2_mod->device_list);
	spin_unlock_irqrestore(&ntv2_mod->device_lock, flags);

	NTV2_MSG_INFO("%s: device probe complete\n", ntv2_dev->name);

	return 0;

disable_device:
	NTV2_MSG_ERROR("%s: device probe failed code %d\n",
				   ntv2_dev->name, result);

	ntv2_device_close(ntv2_dev);
	pci_disable_device(pdev);
	return result;
}

static int ntv2_suspend(struct pci_dev *pdev, pm_message_t state)
{
	struct ntv2_device *ntv2_dev = dev_get_drvdata(&pdev->dev);

	NTV2_MSG_INFO("%s: suspend device\n", ntv2_dev->name);

	ntv2_device_suspend(ntv2_dev);
/*
	pci_disable_device(pdev);
	pci_save_state(pdev);
*/ 
	return 0;
}

static int ntv2_resume(struct pci_dev *pdev)
{
	struct ntv2_device *ntv2_dev = dev_get_drvdata(&pdev->dev);

	NTV2_MSG_INFO("%s: resume device\n", ntv2_dev->name);
/*
	pci_restore_state(pdev);
	pci_enable_device(pdev);
	pci_set_master(pdev);
*/
	ntv2_device_resume(ntv2_dev);

	return 0;
}

static void ntv2_remove(struct pci_dev *pdev)
{
	struct ntv2_device *ntv2_dev = dev_get_drvdata(&pdev->dev);
	struct ntv2_module *ntv2_mod = ntv2_dev->ntv2_mod;
	unsigned long flags;

	NTV2_MSG_INFO("%s: remove device\n", ntv2_dev->name);

	spin_lock_irqsave(&ntv2_mod->device_lock, flags);
	list_del_init(&ntv2_dev->list);
	spin_unlock_irqrestore(&ntv2_mod->device_lock, flags);
	
	ntv2_device_close(ntv2_dev);
	pci_disable_device(pdev);
}

static void ntv2_shutdown(struct pci_dev *pdev)
{
	struct ntv2_device *ntv2_dev = dev_get_drvdata(&pdev->dev);

	NTV2_MSG_INFO("%s: shutdown\n", ntv2_dev->name);
}

static pci_ers_result_t ntv2_pci_error_detected(struct pci_dev *pdev, enum pci_channel_state state)
{
	struct ntv2_device *ntv2_dev = dev_get_drvdata(&pdev->dev);

	NTV2_MSG_ERROR("%s: pci error state %d\n", ntv2_dev->name, state);

	/* stop pci access? */

	if (state == pci_channel_io_perm_failure)
		return PCI_ERS_RESULT_DISCONNECT;
	if (state == pci_channel_io_frozen)
		return PCI_ERS_RESULT_NEED_RESET;
	return PCI_ERS_RESULT_CAN_RECOVER;
}

static pci_ers_result_t ntv2_pci_slot_reset(struct pci_dev *pdev)
{
	struct ntv2_device *ntv2_dev = dev_get_drvdata(&pdev->dev);

	NTV2_MSG_INFO("%s: pci slot reset\n", ntv2_dev->name);
	return 0;
}

static void ntv2_pci_error_resume(struct pci_dev *pdev)
{
	struct ntv2_device *ntv2_dev = dev_get_drvdata(&pdev->dev);

	NTV2_MSG_INFO("%s: pci resume\n", ntv2_dev->name);
}

static struct uart_driver ntv2_uart_driver = {
	.owner			= THIS_MODULE,
	.driver_name	= NTV2_MODULE_NAME,
	.dev_name		= NTV2_TTY_NAME,
	.nr				= NTV2_MAX_UARTS,
};

static const struct pci_device_id ntv2_pci_tbl[] = {
	{
	   NTV2_VENDOR_ID, NTV2_DEVICE_ID_KONA4,
	   PCI_ANY_ID, PCI_ANY_ID,
	   0, 0,
	   0
	},
	{
	   NTV2_VENDOR_ID, NTV2_DEVICE_ID_CORVID88,
	   PCI_ANY_ID, PCI_ANY_ID,
	   0, 0,
	   0
	},
	{
	   NTV2_VENDOR_ID, NTV2_DEVICE_ID_CORVID44,
	   PCI_ANY_ID, PCI_ANY_ID,
	   0, 0,
	   0
	},
	{
	   NTV2_VENDOR_ID, NTV2_DEVICE_ID_CORVIDHDBT,
	   PCI_ANY_ID, PCI_ANY_ID,
	   0, 0,
	   0
	},
	{ 0, 0, 0, 0, 0, 0, 0 }
};
MODULE_DEVICE_TABLE(pci, ntv2_pci_tbl);

static const struct pci_error_handlers ntv2_pci_errors = {
	.error_detected	= ntv2_pci_error_detected,
	.slot_reset		= ntv2_pci_slot_reset,
	.resume			= ntv2_pci_error_resume,
};

static struct pci_driver ntv2_pci_driver = {
	.name			= NTV2_MODULE_NAME,
	.id_table		= ntv2_pci_tbl,
	.probe			= ntv2_probe,
	.suspend		= ntv2_suspend,
	.resume			= ntv2_resume,
	.remove			= ntv2_remove,
	.err_handler	= &ntv2_pci_errors,
	.shutdown		= ntv2_shutdown,
};

static int __init ntv2_module_init(void)
{
	struct ntv2_module *ntv2_mod = NULL;
	int result;

	/* initialize device module info */
	ntv2_module_initialize();
	ntv2_mod = ntv2_module_info();

	NTV2_MSG_INFO("%s: module init version %s\n", ntv2_mod->name, ntv2_mod->version);

	/* register uart driver */
	result = uart_register_driver(&ntv2_uart_driver);
	if (result < 0) {
		NTV2_MSG_ERROR("%s: *error* uart_register_driver failed code %d\n",
					   ntv2_mod->name, result);
		return result;
	}
	ntv2_mod->uart_driver = &ntv2_uart_driver;

	/* register character driver */
	result = alloc_chrdev_region(&ntv2_mod->cdev_number, 0, ntv2_mod->cdev_max, ntv2_mod->cdev_name);
	if (result < 0) {
		NTV2_MSG_ERROR("%s: *error*  alloc_chrdev_region failed code %d\n",
					   ntv2_mod->name, result);
		return result;
	}

	/* probe the devices */
	result = pci_register_driver(&ntv2_pci_driver);
	if (result < 0) {
		NTV2_MSG_ERROR("%s: *error* pci_register_driver failed code %d\n",
					   ntv2_mod->name, result);
		uart_unregister_driver(&ntv2_uart_driver);
		return result;
	}
	if (atomic_read(&ntv2_mod->device_index) == 0)
		NTV2_MSG_INFO("%s: no ntv2 boards found\n", ntv2_mod->name);

	NTV2_MSG_INFO("%s: module init complete\n", ntv2_mod->name);
	
	return 0;
}

static void __exit ntv2_module_exit(void)
{
	struct ntv2_module *ntv2_mod = ntv2_module_info();

	NTV2_MSG_INFO("%s: module exit start\n", ntv2_mod->name);

   	pci_unregister_driver(&ntv2_pci_driver);
	unregister_chrdev_region(ntv2_mod->cdev_number, ntv2_mod->cdev_max);
	uart_unregister_driver(&ntv2_uart_driver);
	ntv2_module_release();

	NTV2_MSG_INFO("%s: module exit complete\n", ntv2_mod->name);
}

module_init(ntv2_module_init);
module_exit(ntv2_module_exit);
