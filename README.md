# virshyt.
# a c based, libvirt using management tool for restarting, stopping, or starting domains in a libvirt environment.
# written for the cis235 class at Pima, because I was too lazy to actually bother with bash scripting...
# offers the ability to check that the domains are actually responding to ssh before continuing.
# uses an insecure connection to a libvirt domain host.
# assumes your /etc/hosts or dns is configured such that libvirt domain name == internet domain name

