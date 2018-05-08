#ifndef DOMAINMANAGER_H
#define DOMAINMANAGER_H

#include <libvirt/libvirt.h>
#include <stdio.h>
#include <stdlib.h>


// this returns a virConnectPtr
// it takes a libvirt domain name,
// and a read/write flag
virConnectPtr getConnectionPtr(const char *name, int rw) {
	virConnectPtr ret = NULL;
	// if rw != 0: open a read write connection
	//       else: open a read connection
	ret = rw != 0 ? virConnectOpen(name) : virConnectOpenReadOnly(name);
	if (ret == NULL)
		goto error;
	return ret;
error:
	fprintf(stderr,"failed to open %s connection to %s\n", rw != 0 ? "read write" : "read", name);
	return NULL;
}

// this returns a virDomainPtr to the given domain name
// it takes a domain name
// and a virConnectPtr
virDomainPtr getDomainPtr(const char *name, virConnectPtr conn) {
	virDomainPtr ret = NULL;
	ret = virDomainLookupByName(conn, name);
	if (ret == NULL)
		goto error;
	return ret;
error:
	fprintf(stderr,"failed to find domain %s\n",name);
	return NULL;

}

// this returns 1 if running, 0 if not.
// it takes a virDomainPtr
int isRunning(virDomainPtr doma) {
	int state = 0;
	if (virDomainGetState(doma,&state,NULL,0) != 0)
		goto error;
	if (state == 4 | state == 1)
		return 1;
	return 0;
error:
	fprintf(stderr,"Failed to get state of domain %s\n",virDomainGetName(doma));
	return -1;
}

// this returns 1 if the domain fails to stop
// 0 if not
int stopDomain(virDomainPtr doma) {
	if(virDomainShutdown(doma) != 0)
		goto error;
	return 0;
error:
	fprintf(stderr,"failed to stop domain %s\n",virDomainGetName(doma));
	return 1;
}

// this returns 1 if the domain fails to start
// 0 if not
int startDomain(virDomainPtr doma) {
	if(virDomainCreate(doma) != 0)
		goto error;
	return 0;
error:
	fprintf(stderr,"failed to start domain %s\n",virDomainGetName(doma));
	return 1;
}


// this should get a domains first network interface
// but it is broken, and is not used anywhere.
char* getDomainInterface(virDomainPtr doma) {
	virDomainInterfacePtr *ifaces = NULL;
	int ifaces_count = 0;
	size_t i;
	char *ret = "";;
	printf("Getting domain interfaces\n");
	if ((ifaces_count = virDomainInterfaceAddresses(doma, &ifaces,
					VIR_DOMAIN_INTERFACE_ADDRESSES_SRC_LEASE, 0)) < 0)
		goto cleanup;
	printf("Got domain interface %s\n",ifaces[0]->name);
	strcpy(ret,ifaces[0]->name);
cleanup:
	printf("Get domain interface failed\n");
	if (ifaces && ifaces_count > 0)
		for (i = 0; i < ifaces_count; i++)
			virDomainInterfaceFree(ifaces[i]);
	free(ifaces);
	return ret;
}

#endif

