/* SPDX-License-Identifier: GPL-2.0 */
#include <linux/bpf.h>
#include <linux/in.h>
#include <bpf/bpf_helpers.h>
#include <bpf/bpf_endian.h>

#include <linux/if_ether.h>

#ifndef memcpy
#define memcpy(dest, src, n) __builtin_memcpy((dest), (src), (n))
#endif

struct {
	__uint(type, BPF_MAP_TYPE_DEVMAP);
	__uint(max_entries, 256);
	__type(key, int);
	__type(value, int);
} tx_port SEC(".maps");

struct {
	__uint(type, BPF_MAP_TYPE_HASH);
	__uint(max_entries, 1);
	__type(key, char[ETH_ALEN]);
	__type(value, char[ETH_ALEN]);
} redirect_params SEC (".maps");

SEC("xdp_redirect_map")
int xdp_redirect_map_func(struct xdp_md *ctx)
{
	void *data_end = (void *)(long)ctx->data_end;
	void *data = (void *)(long)ctx->data;
	struct ethhdr *eth;
	unsigned char *dst;

	eth = data;

	if (data + sizeof(*eth) > data_end)
		return XDP_PASS;

	/* Do we know where to redirect this packet? */
	dst = bpf_map_lookup_elem(&redirect_params, eth->h_source);
	if (!dst)
		return XDP_PASS;

	/* Set a proper destination address */
	memcpy(eth->h_dest, dst, ETH_ALEN);
	return bpf_redirect_map(&tx_port, 0, 0);
}

char _license[] SEC("license") = "GPL";
