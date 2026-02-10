/* mydriver_main.c:  major functions of mydriver */

#import <kernserv/kern_server_types.h>
#import <kernserv/prototypes.h>
#import <ansi/string.h>
#import "../Library/mydefs.h"

kern_server_t instance;

/* Pointer to hold local data in the kernel_task's vm */
static user_data_t local_data;

/* mydriver_init:  Called when mydriver is loaded. */
void mydriver_init(void)
{
    kern_return_t r;
    port_t kernel_task = kern_serv_kernel_task_port();	

    //
    // Get a page of publically readable vm to be used to access the mig
    // messages out of line data.  Also used to hold data across import
    // and export calls.
    //
    r = vm_allocate
	(kernel_task, (vm_address_t *) &local_data, page_size, TRUE);
    if (r != KERN_SUCCESS)
    {
	printf("Call to vm_allocate failed %d\n", r);
	local_data = NULL;
    }

    //
    // Clear the newly acquired memory
    //
    memset(local_data, '\0', page_size);
    
    printf("My driver loaded\n");
}


/* mydriver_signoff:  Called when mydriver is unloaded. */
void mydriver_signoff(void)
{
    kern_return_t r;
    port_t kernel_task = kern_serv_kernel_task_port();	

    // Deallocate the kernel vm's local_data buffer
    r = vm_deallocate(kernel_task, (vm_address_t) local_data, page_size);
    if (r != KERN_SUCCESS)
    {
	printf("Call to vm_deallocate failed %d\n", r);
	local_data = NULL;
    }
    
    printf("My driver unloaded\n\n");
}

/* mydriver_import:  Called by mydriver_server, which was created by MiG. */
kern_return_t mydriver_import
    (port_t server, in_user_data_t user_data, unsigned int length)
{
    kern_return_t r;
    user_data_t tmp;
    port_t kernel_task = kern_serv_kernel_task_port();	

    if (!local_data)
	return KERN_FAILURE;

    //
    // Warning:  Loadable kernel servers run outside of the kernel task,
    // even though they use the kernel address map.
    //
    // This means that we have to get the task_self's user_data into the
    // kernel's vm.  To do this we have to, counterintuitively, write the
    // received data into our previously acquired vm page (kernel_task).
    //
    r = vm_write((vm_task_t) kernel_task, (vm_address_t) local_data,
	(pointer_t) user_data, page_size);

    if (r == KERN_SUCCESS)
    {
	tmp = local_data;
	printf("\nTrying to access data.\n");
	printf("First  data element is: %c\n", *tmp++);
	printf("Second data element is: %c\n", *tmp++);
	printf("Third  data element is: %c\n", *tmp++);
	printf("Fourth data element is: %c\n", *tmp++);
	printf("Fifth  data element is: %c\n", *tmp++);
	
	printf("Contents of data are: %s\n", local_data);
    }
    else switch(r)
    {
    case KERN_INVALID_ARGUMENT:
	    printf("vm_write: KERN_INVALID_ARGUMENT %d\n", r);
	    break;
    case KERN_PROTECTION_FAILURE:
	    printf("vm_write: KERN_PROTECTION_FAILURE %d\n", r);
	    break;
    case KERN_INVALID_ADDRESS:
	    printf("vm_write: KERN_INVALID_ADDRESS %d\n", r);
	    break;
    default:
	    printf("Call to vm_write failed %d\n", r);
    }
    
    // 
    // Remove the out_of_band data from the kernel server task's vm.
    // Otherwise we would have a memory leak across the mach message interface.
    //
    r = vm_deallocate(task_self(), (vm_address_t) user_data, page_size);
    if (r != KERN_SUCCESS)
	printf("Call to vm_deallocate user_data failed %d\n", r);
    
    return r;
}	

/* mydriver_export:  Called by mydriver_server, which was created by MiG. */
kern_return_t mydriver_export
    (port_t server, out_user_data_t *user_data, unsigned int *length)
{
    kern_return_t r;
    port_t kernel_task = kern_serv_kernel_task_port();
    unsigned int tmp;
    
    if (!local_data)
    {
	// Get some vm for the mig out of band data to deallocate
	(void) vm_allocate
	    (task_self(), (vm_address_t *) user_data, page_size, TRUE);
	
	return KERN_FAILURE;
    }
    
    // Tack an ACK on the end of the imported data
    strcat(local_data, "ACK");
    *length = strlen(local_data) + 1;
    printf("\nData is: %s\n", local_data);

    //
    // Warning:  Loadable kernel servers run outside of the kernel task,
    // even though they use the kernel address map.
    //
    // This means that we have to load the task_self's user_data pointer from
    // the kernel's vm.  To do this we have to, counterintuitively again,
    // vm_read the local_data into our task's vm, NB we do not have to
    // previously allocate the vm.  The vm_read data will be deallocated
    // automatically by the mach message system as the type was declared
    // with dealloc on the mig type.
    //
    r = vm_read((vm_task_t) kernel_task, (vm_address_t) local_data,
	    page_size, (pointer_t *) user_data, &tmp);
    if (r != KERN_SUCCESS)
    {
	// Get some vm for the mig out of band data to deallocate
	(void) vm_allocate
	    (task_self(), (vm_address_t *) user_data, page_size, TRUE);
	
	switch (r)
	{
	case KERN_INVALID_ARGUMENT:
		printf("vm_read: KERN_INVALID_ARGUMENT %d\n", r);	break;
	case KERN_NO_SPACE:
		printf("vm_read: KERN_NO_SPACE %d\n", r);		break;
	case KERN_PROTECTION_FAILURE:
		printf("vm_read: KERN_PROTECTION_FAILURE %d\n", r);	break;
	case KERN_INVALID_ADDRESS:
		printf("vm_read: KERN_INVALID_ADDRESS %d\n", r);	break;
	default: printf("Call to vm_read failed: %d\n", r);		break;
	}
    }

    //
    // No matter what path we take through this function we always have some
    // disposable vm in task_self's memory for dealloc-ing.
    //
    return r;
}
