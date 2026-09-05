#ifndef __services_h
#define __services_h

/**
This file provides headers for all functions and globals provided by the top half. Sometimes the functions declared by this file are actually implemented in the top half and sometimes this is just to declare a kernel function that we wish to use. We cannot include the kernel headers files in C++ code since they confuse the hell out of the C++ compiler and so we resort to declaring these functions here.
*/

typedef int (*func_int_pvoid)(void*);
typedef void (*func_void_pvoid)(void*);
typedef unsigned char (*func_unsigned_char_pvoid_pvoid)(void*,void*);
typedef void (*func_void_pvoid_pvoid_pvoid_pvoid)(void*,void*,void*,void*);

#ifdef __cplusplus
extern "C" {
#endif // __cplusplus

// empty class
void service_empty(void);

// infrastructure calls
void service_init(void);
void service_finish(void);

// general module name API's
const char* service_modname(void);
void service_bug(void);
void service_stacktrace(void);

// printing and debugging
int service_printk(const char* fmt,...) __attribute__((format (printf, 1, 2)));
int service_vprintk(const char* fmt,__builtin_va_list list);
int service_puts(const char* s);

// These are the pair of function you should use to register/unregister your
// device with the linux kernel
void service_register_class(void);
void service_destroy_class(void);
//void* service_register_dev(callback c,void* data,const char* name) __attribute__((warn_unused_result));
void service_unregister_dev(void* opaque);

// dma API
void* service_dma_alloc_coherent(unsigned int* device_pointer,unsigned int size) __attribute__((warn_unused_result));
void service_dma_free_coherent(void* vaddr,unsigned int device_address,unsigned int size);

// user interaction APIs
int service_copy_from_user(void* to,const void* from,unsigned int size) __attribute__((warn_unused_result));
int service_copy_to_user(void* to,const void* from,unsigned int size) __attribute__((warn_unused_result));
int service_in_passive(void) __attribute__((warn_unused_result));
int service_in_interrupt(void) __attribute__((warn_unused_result));
int service_in_irq(void) __attribute__((warn_unused_result));

// memory allocation APIs
void service_memcpy(const void* to,const void* from,unsigned int size);
void* service_malloc(unsigned int) __attribute__((warn_unused_result));
void service_free(void* pointer);
void service_mdebug(void);

// string functions
void service_strncpy(char* dst,const char* src,unsigned int size);
char* service_strcpy(char* dst,const char* src);
int service_snprintf(char* buf,unsigned int size,const char* fmt,...) __attribute__((format (printf, 3, 4)));
int service_sprintf(char* dst,const char* fmt,...) __attribute__((format (printf, 2, 3)));
int service_vsnprintf(char* buf,unsigned int size,const char* fmt,__builtin_va_list list);
int service_vsprintf(char* buf,const char* fmt,__builtin_va_list list);
int service_strlen(const char* str);
int service_strncmp(const char *s1, const char *s2, unsigned int size);
char *service_strcat(char *dest, const char *src);

// timing functions
unsigned long long service_get_cycles(void) __attribute__((warn_unused_result));
unsigned long long service_get_freq(void) __attribute__((warn_unused_result));
void service_msleep(unsigned long mili);
void service_udelay(unsigned long micro);
unsigned long service_get_mic_diff(unsigned long long before,unsigned long long after);

// pci function
void* service_pci_get_dev(void) __attribute__((warn_unused_result));
int service_pci_register(func_int_pvoid probe,func_void_pvoid remove);
void service_pci_unregister(void);
void service_pci_set_user_data(void* vpci_dev,void* data);
void* service_pci_get_user_data(void* vpci_dev);
int service_pci_enable_and_request(void* vpci_dev);
void service_pci_release_and_disable(void* vpci_dev);
unsigned long service_pci_resource_start(void* vpci_dev,const int bar);
unsigned long service_pci_resource_len(void* vpci_dev,const int bar);
void* service_ioremap(unsigned long start,unsigned long len) __attribute__((warn_unused_result));
void service_iounmap(void* pointer);
void service_pci_read_header(void* header,unsigned int offset,unsigned int count,void* vpci_dev);
void service_pci_write_header(void* header,unsigned int offset,unsigned int count,void* vpci_dev);
unsigned int service_pci_get_irqnum(void* vpci_dev);
int service_pci_init(void) __attribute__((warn_unused_result));
int service_pci_get_msix_irq_num(void);

// spin lock API
void* service_spinlock_init(void) __attribute__((warn_unused_result));
void service_spinlock_lock(void* lock);
void service_spinlock_unlock(void* lock);
void service_spinlock_free(void* lock);
void service_spinlock_irq_lock(void* lock);
void service_spinlock_irq_unlock(void* lock);

// mempool API
void* service_mempool_create(const char* name,unsigned int size,unsigned int warmup) __attribute__((warn_unused_result));
void* service_mempool_alloc(void* p) __attribute__((warn_unused_result));
void service_mempool_free(void* p,void* obj);
void service_mempool_destroy(void* p);

// IRQ API

typedef func_unsigned_char_pvoid_pvoid user_irq_handler;
void* service_register_irq(unsigned int irqnum,user_irq_handler irq_handler,void* context) __attribute__((warn_unused_result));
void service_free_irq(void* handle);
void service_irq_disable(unsigned int irq);
void service_irq_enable(unsigned int irq);

// Memory addressing issues
unsigned int service_get_page_size(void) __attribute__((warn_unused_result));
unsigned long service_virt_to_phys(void* virt) __attribute__((warn_unused_result));
void service_zeromem(void* addr,unsigned int size);
void service_memset(void* addr,int c,unsigned int size);

// IO operations
unsigned char service_inb(unsigned int port);
unsigned short service_inw(unsigned int port);
unsigned int service_inl(unsigned int port);
void service_insb(unsigned int port,void* addr,unsigned long count);
void service_insw(unsigned int port,void* addr,unsigned long count);
void service_insl(unsigned int port,void* addr,unsigned long count);
void service_outb(unsigned char val,unsigned port);
void service_outw(unsigned short val,unsigned port);
void service_outl(unsigned int val,unsigned port);
void service_outsb(unsigned int port,void* addr,unsigned long count);
void service_outsw(unsigned int port,void* addr,unsigned long count);
void service_outsl(unsigned int port,void* addr,unsigned long count);
unsigned char service_ioread8(void* addr);
unsigned short service_ioread16(void* addr);
unsigned int service_ioread32(void* addr);
void service_iowrite8(unsigned char data,void* addr);
void service_iowrite16(unsigned short data,void* addr);
void service_iowrite32(unsigned int data,void* addr);
void service_ioread8_rep(void* addr,void* buf,unsigned long count);
void service_ioread16_rep(void* addr,void* buf,unsigned long count);
void service_ioread32_rep(void* addr,void* buf,unsigned long count);
void service_iowrite8_rep(void* addr,void* buf,unsigned long count);
void service_iowrite16_rep(void* addr,void* buf,unsigned long count);
void service_iowrite32_rep(void* addr,void* buf,unsigned long count);
void service_smp_mb(void);

// CACHE functions
void* service_cache_create(const char* name,unsigned int size,unsigned int warmup) __attribute__((warn_unused_result));
void* service_cache_alloc(void* cache) __attribute__((warn_unused_result));
void service_cache_free(void* cache,void* obj);
void service_cache_destroy(void* cache);

//User memory allocation functions
void* service_user_to_kernel_adr(void* user) __attribute__((warn_unused_result));
//int service_put_vma_data(struct vm_area_struct* vma,void* adr,mem_type type) __attribute__((warn_unused_result));
void* service_get_vma_adr(struct vm_area_struct* vma) __attribute__((warn_unused_result));
int service_is_our_vma(struct vm_area_struct* vma) __attribute__((warn_unused_result));
int service_is_our_uadr(unsigned int uadr) __attribute__((warn_unused_result));
int service_free_vma(struct vm_area_struct* vma) __attribute__((warn_unused_result));
unsigned int service_round_to_page(unsigned int size);
void* service_alloc_user_mem(unsigned int size) __attribute__((warn_unused_result));
void service_free_user_mem(void* kptr,unsigned int size);

// Tasklet stuff
typedef func_void_pvoid_pvoid_pvoid_pvoid user_tasklet_handler;
void* service_tasklet_init(user_tasklet_handler func) __attribute__((warn_unused_result));
void service_tasklet_schedule(void* handle,void* param1,void* param2,void* param3,void* param4);
void service_tasklet_free(void* handle);
void service_tasklet_disable(void);
void service_tasklet_enable(void);

// List stuff
void* service_list_create(void);
void service_list_destroy(void* ptr);
int service_list_isempty(void* lptr);
void* service_list_iter_init(void* lptr);
int service_list_iter_isover(void* lptr,void* iter);
void* service_list_iter_getval(void* lptr,void* iter);
void* service_list_iter_next(void* lptr,void* iter);
void service_list_iter_del(void* lptr,void* iter);
void* service_list_pop(void* lptr);
void service_list_add(void* lptr,void* ptr);

// map to user stuff
unsigned long service_map_to_user(void* filp,void* kptr,unsigned int size,bool use_remap,bool ro,bool reserve,unsigned int type) __attribute__((warn_unused_result));
int service_unmap(unsigned long uadr,bool unreserve,void** pkadr,unsigned int* psize,bool showMercy) __attribute__((warn_unused_result));
int kern_mmap(struct file *filp, struct vm_area_struct *vma);
void service_print_page(struct page *page);
void service_buffer_reserve(void* kadr,unsigned int size);
void service_buffer_unreserve(void* kadr,unsigned int size);
void service_buffer_print(void* kadr,unsigned int size);
void service_buffer_use(void* kadr,unsigned int size);
void service_buffer_unuse(void* kadr,unsigned int size);
void* service_uadr_to_kadr(unsigned int uadr) __attribute__((warn_unused_result));

// get user pages
int service_get_user_pages(void* b_pointer,unsigned int b_size,void** pptr,void** pvptr,unsigned int* pnr_pages,void** ppages) __attribute__((warn_unused_result));
void service_release_pages(void* vptr,void* ppages,unsigned int nr_pages);

// events
int service_events_drv_init(void) __attribute__((warn_unused_result));
void service_events_drv_fini(void);
int service_events_create(void) __attribute__((warn_unused_result));
int service_events_set(unsigned int num) __attribute__((warn_unused_result));
int service_events_reset(unsigned int num) __attribute__((warn_unused_result));
//int service_events_wait_forever(WFSO* wfso) __attribute__((warn_unused_result));
//int service_events_wait_timeout(WFSO* wfso) __attribute__((warn_unused_result));
int service_events_close(unsigned int num) __attribute__((warn_unused_result));

// file data
void service_filedata_setup(struct file* filp);
void service_filedata_teardown(struct file* filp);

// completion stuff
void* service_completion_malloc(void) __attribute__((warn_unused_result));
void service_completion_free(void* vcomp);
void service_completion_init(void* vcomp);
void service_completion_wait_forever_interruptible(void* vcomp);
unsigned long service_completion_wait_timeout_interruptible(void* vcomp,unsigned int msecs) __attribute__((warn_unused_result));
void service_completion_wait_forever(void* vcomp);
void service_completion_complete_all(void* vcomp);

// registry stuff
int service_registry_entry(const char* key);
void service_registry_set(const char* key,const char* val);
int service_registry_read(void);
int service_registry_init(void) __attribute__((warn_unused_result));
int service_registry_fini(void) __attribute__((warn_unused_result));
int service_registry_write(void) __attribute__((warn_unused_result));
int service_registry_flush(void) __attribute__((warn_unused_result));
char* service_registry_get(const char* key);

char* service_registry_get_string(const char* key,char* defval) __attribute__((warn_unused_result));
int service_registry_get_bool(const char* key,int defval) __attribute__((warn_unused_result));
int service_registry_get_int(const char* key,int defval) __attribute__((warn_unused_result));
unsigned int service_registry_get_uint(const char* key,unsigned int defval) __attribute__((warn_unused_result));

// atoi atuff
int service_atoi(const char *name) __attribute__((warn_unused_result));
unsigned int service_atou(const char *name) __attribute__((warn_unused_result));

// dmam stuff
int service_dmam_init(void) __attribute__((warn_unused_result));
int service_dmam_fini(void) __attribute__((warn_unused_result));
void* service_dmam_alloc_coherent(unsigned int* device_pointer,unsigned int size) __attribute__((warn_unused_result));
void service_dmam_free_coherent(void* vaddr,unsigned int device_address,unsigned int size);

#ifdef __cplusplus
}
#endif // __cplusplus

#endif // __services_h
