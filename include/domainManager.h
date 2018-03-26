#ifndef DOMAINMANAGER_H
#define DOMAINMANAGER_H

#include <libvirt/libvirt.h>
#include <stdio.h>
#include <stdlib.h>

virConnectPtr getConnectionPtr(const char *name, int rw) {
	virConnectPtr ret = NULL;
	ret = rw != 0 ? virConnectOpen(name) : virConnectOpenReadOnly(name);
	if (ret == NULL)
		goto error;
	return ret;
error:
	fprintf(stderr,"failed to open %s connection to %s\n", rw != 0 ? "read write" : "read", name);
	return NULL;
}

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

int stopDomain(virDomainPtr doma) {
	int ret = 0;

	ret = virDomainShutdown(doma);
	if (ret != 0)
		goto error;
	return ret;
error:
	fprintf(stderr,"failed to stop domain %s\n",virDomainGetName(doma));
	return 1;
}

int startDomain(virDomainPtr doma) {
	int ret = 0;
	
	ret = virDomainCreate(doma);
	if (ret != 0)
		goto error;
	return ret;
error:
	fprintf(stderr,"failed to start domain %s\n",virDomainGetName(doma));
	return 1;
}

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

