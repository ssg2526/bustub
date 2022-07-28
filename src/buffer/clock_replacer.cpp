//===----------------------------------------------------------------------===//
//
//                         BusTub
//
// clock_replacer.cpp
//
// Identification: src/buffer/clock_replacer.cpp
//
// Copyright (c) 2015-2019, Carnegie Mellon University Database Group
//
//===----------------------------------------------------------------------===//

#include "buffer/clock_replacer.h"
#include "common/logger.h"

namespace bustub {

ClockReplacer::ClockReplacer(size_t num_pages) {
    using std::vector;
    in_replacer = vector<bool>(num_pages, false);
    ref_bits = vector<bool>(num_pages, false);
    buffer_pool_size = num_pages;
    hand = 0;
    replacer_size = 0;
}

ClockReplacer::~ClockReplacer() = default;

auto ClockReplacer::Victim(frame_id_t *frame_id) -> bool {
    latch.lock();
    if(replacer_size <= 0){
        latch.unlock();
        return false;
    }
    while(true){
        if(!in_replacer[hand]){
            hand = (hand+1)%buffer_pool_size;
            continue;
        }
        if(ref_bits[hand]){
            ref_bits[hand] = false;
            hand = (hand+1)%buffer_pool_size;
            continue;
        }
            *frame_id = hand;
            in_replacer[hand] = false;
            ref_bits[hand] = false;
            replacer_size--;
            break;
    }
    latch.unlock();
    return true;
}

void ClockReplacer::Pin(frame_id_t frame_id) {
    latch.lock();
    if(in_replacer[frame_id]){
        in_replacer[frame_id] = false;
        ref_bits[frame_id] = false;
        replacer_size--;
    }
    latch.unlock();
}

void ClockReplacer::Unpin(frame_id_t frame_id) {
    latch.lock();
    if(!in_replacer[frame_id]){
        ref_bits[frame_id] = true;
        in_replacer[frame_id] = true;
        replacer_size++;
    }
    latch.unlock();
}

auto ClockReplacer::Size() -> size_t { 
    return replacer_size; 
}

}  // namespace bustub
