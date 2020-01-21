//
// zero - pre-emptive multitasking kernel for AVR
//
// Techno Cosmic Research Institute    Dirk Mahoney           dirk@tcri.com.au
// Catchpole Robotics                  Christian Catchpole    christian@catchpole.net
//


#include <string.h>
#include <avr/io.h>

#include "pagemanager.h"
#include "util.h"


using namespace zero;


// bit-field manipulation macros
#define BF_BYTE(b)          ((int)(b) >> 3)
#define BF_BIT(b)           ((int)(b) & 0b111)

#define BF_SET(bf,b)        bf[BF_BYTE(b)] |= (1 << BF_BIT(b))
#define BF_CLR(bf,b)        bf[BF_BYTE(b)] &= ~(1 << BF_BIT(b))
#define BF_TST(bf,b)        (bf[BF_BYTE(b)] & (1 << BF_BIT(b)))

// friendly names for the bit-field manipulators
#define IS_PAGE_AVAIL(b)    (!BF_TST(_memoryMap,b))
#define MARK_AS_USED(b)     (BF_SET(_memoryMap,b))
#define MARK_AS_FREE(b)     (BF_CLR(_memoryMap,b))


// Has the page been marked as used, or is it free?
template <uint16_t PAGE_COUNT>
bool PageManager<PAGE_COUNT>::isPageAvailable(const uint16_t pageNumber)
{
    return IS_PAGE_AVAIL(pageNumber);
}


// Mark the supplied page as free
template <uint16_t PAGE_COUNT>
void PageManager<PAGE_COUNT>::markAsFree(const uint16_t pageNumber)
{
    MARK_AS_FREE(pageNumber);
}


// Mark the supplied page as used
template <uint16_t PAGE_COUNT>
void PageManager<PAGE_COUNT>::markAsUsed(const uint16_t pageNumber)
{
    MARK_AS_USED(pageNumber);
}


// Returns the number of pages being managed
template <uint16_t PAGE_COUNT>
uint16_t PageManager<PAGE_COUNT>::getTotalPageCount()
{
    return PAGE_COUNT;
}


// Returns the number of currently available pages
template <uint16_t PAGE_COUNT>
uint16_t PageManager<PAGE_COUNT>::getFreePageCount()
{
    uint16_t rc = 0UL;

    for (uint16_t i = 0; i < getTotalPageCount(); i++) {
        if (isPageAvailable(i)) {
            rc++;
        }
    }
    
    return rc;
}


// Returns the number of currently allocated pages
template <uint16_t PAGE_COUNT>
uint16_t PageManager<PAGE_COUNT>::getUsedPageCount()
{
    return getTotalPageCount() - getFreePageCount();
}


// search strategy function prototypes
static int16_t getPageForSearchStep_TopDown(const uint16_t step, const uint16_t totalPages);
static int16_t getPageForSearchStep_BottomUp(const uint16_t step, const uint16_t totalPages);


// search strategy - BottomUp starts searching the allocation table at the bottom and works up
static int16_t getPageForSearchStep_BottomUp(
    const uint16_t step,
    const uint16_t totalPages)
{
    return step;
}


// search strategy - TopDown starts searching the allocation table at the top and works down
static int16_t getPageForSearchStep_TopDown(
    const uint16_t step,
    const uint16_t totalPages)
{
    return totalPages - (step + 1);
}


// set up the vector table for the search strategies
static int16_t (*_strategies[])(const uint16_t, const uint16_t) = {
    getPageForSearchStep_TopDown,
    getPageForSearchStep_BottomUp,
};


// This is the main workhorse for the memory allocator. Using only the search strategy supplied,
// find a supplied number of continguously available pages.
template <uint16_t PAGE_COUNT>
int16_t PageManager<PAGE_COUNT>::findFreePages(
    const uint16_t numPagesRequired,
    const memory::SearchStrategy strat)
{
    uint16_t startPage = (uint16_t) -1;
    uint16_t pageCount = 0;

    for (uint16_t curStep = 0; curStep < PAGE_COUNT; curStep++) {
        const uint16_t curPage = _strategies[strat](curStep, PAGE_COUNT);

        // if the search strategy no longer
        // has any more pages in its scope
        if (curPage == (uint16_t) -1) {
            break;
        }

        // But if that page was free..
        if (isPageAvailable(curPage)) {
            // we have one more page than we had before
            pageCount++;

            // if we didn't have a startPage, we do now
            if (startPage == (uint16_t) -1) {
                startPage = curPage;
            }

            // if we've found the right number of pages, stop looking
            if (pageCount == numPagesRequired) {
                startPage = MIN(startPage, curPage);
                break;
            }
        }
        else {
            // wasn't free? start the search from scratch
            startPage = -1;
            pageCount = 0;
        }
    }

    // if we couldn't find right number of pages
    if (pageCount < numPagesRequired) {
        return -1;
    }

    // if we get here, then we were successful at
    // finding what the caller wanted
    return startPage;
}


#include "pagemanager_classes.h"