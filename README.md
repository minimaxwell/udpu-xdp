# udpu-xdp
Small XDP helpers tested un uDPU, derived from xdp-tutorials

# Building

You'll need a toolchain that has recent kernel headers, libelf and zlib installed.
To compile, simply run the make command :

 `make`
 
 You'll then need to install the tools populated in the xdp-redirect directory :
  - xdp_lader
  - xdp_redirect_setup
  - xdp_redirect.o
  
# Using on the uDPU

First, bring the 2 interfaces up :

`ip l s eth0 up`

`ip l s eth1 up`

Mount the BPF filesystem so that the xdp programs can be pinned to each interface :

`mount bpffs /sys/fs/bpf -t bpf`

Then, attach the XDP programs to each interface :

`./xdp_loader -d eth0 --filename ./xdp_redirect.o --progsec xdp_redirect_map --force`

`./xdp_loader -d eth1 --filename ./xdp_redirect.o --progsec xdp_redirect_map --force`

For the next part, we need to indicate the MAC addresses of the link-partner of each interface,
that can be retrieved using `ip neigh` for example.

Here in this setup, lp-0-addr represents the mac-addr of the device connected to eth0, and lp-1-addr is the mac-addr of the device connected to eth1.

`./xdp_redirect_setup -d eth0 -r eth1 -L <lp-0-addr> -R <lp-1-addr>`

`./xdp_redirect_setup -d eth1 -r eth0 -R <lp-0-addr> -L <lp-1-addr>`

At that point the redirection is setup, however it's still not functionnal yet. Mvneta has built-in MAC filtering
capabilities, and will silently drop frames that aren't addressed for the receiving port. To solve that, simply set
both interfaces to promiscuous mode :

`ip link set dev eth0 promisc on`

`ip link set dev eth1 promisc on`

The setup is now complete !

You can consult the XDP statistics using `ethtool -S eth0/1`.
