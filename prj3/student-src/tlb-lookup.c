#include <stdlib.h>
#include <stdio.h>
#include "tlb.h"
#include "pagetable.h"
#include "global.h" /* for tlb_size */
#include "statistics.h"

/*******************************************************************************
 * Looks up an address in the TLB. If no entry is found, attempts to access the
 * current page table via cpu_pagetable_lookup().
 *
 * @param vpn The virtual page number to lookup.
 * @param write If the access is a write, this is 1. Otherwise, it is 0.
 * @return The physical frame number of the page we are accessing.
 */
pfn_t tlb_lookup(vpn_t vpn, int write) {
   pfn_t pfn;
    // we are using a write through policy
   /* 
    * FIX ME : Step 6
    */

   /* 
    * Search the TLB for the given VPN. Make sure to increment count_tlbhits if
    * it was a hit!
    */
    int i;
    for (i = 0; i < tlb_size; i++) {
        if (tlb[i].vpn == vpn && tlb[i].valid) {
            //increase tibhits
            count_tlbhits++;
            //get pfn
            pfn = tlb[i].pfn;
            //update tlb
            tlb[i].dirty = write;
            tlb[i].used = 1;
            // update information in the pagetable as well
            current_pagetable[vpn].dirty = write;
            current_pagetable[vpn].valid = 1;
            current_pagetable[vpn].used = 1;

            return pfn;
        }
    }
   /* If it does not exist (it was not a hit), call the page table reader */
   pfn = pagetable_lookup(vpn, write);

   /* 
    * Replace an entry in the TLB if we missed. Pick invalid entries first,
    * then do a clock-sweep to find a victim.
    */
    // We are using the clock sweep algorithm

    for (i = 0; i < tlb_size; i++) {

        if (!tlb[i].valid) {

            // update tlb
            tlb[i].vpn = vpn;
            tlb[i].pfn = pfn;
            tlb[i].dirty = write;
            tlb[i].used = 1;
            tlb[i].valid = 1;
            // update current table
            current_pagetable[vpn].used = 1;
            current_pagetable[vpn].valid = 1;
            current_pagetable[vpn].pfn = pfn;
            return pfn;

        }
    }

    for (i = 0; i < tlb_size + 1; i++) {
        if (tlb[i%tlb_size].used) {

            tlb[i%tlb_size].used = 0;

        } else {
            //update tlb
            tlb[i%tlb_size].used = 1;
            tlb[i%tlb_size].vpn = vpn;
            tlb[i%tlb_size].pfn = pfn;
            tlb[i%tlb_size].dirty = write;
            tlb[i%tlb_size].valid = 1;
            //update the current pagetable
            current_pagetable[vpn].used = 1;
            current_pagetable[vpn].valid = 1;
            current_pagetable[vpn].pfn = pfn;

            return pfn;

        }
    }
   /*
    * Perform TLB house keeping. This means marking the found TLB entry as
    * accessed and if we had a write, dirty. We also need to update the page
    * table in memory with the same data.
    *
    * We'll assume that this write is scheduled and the CPU doesn't actually
    * have to wait for it to finish (there wouldn't be much point to a TLB if
    * we didn't!).
    */

   return pfn;
}

