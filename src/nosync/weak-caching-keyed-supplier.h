// This file is part of libnosync library. See LICENSE file for license details.
#ifndef NOSYNC__WEAK_CACHING_KEYED_SUPPLIER_H
#define NOSYNC__WEAK_CACHING_KEYED_SUPPLIER_H

#include <functional>
#include <memory>


namespace nosync
{

template<typename Key, typename Value>
std::function<std::shared_ptr<Value>(const Key &)> make_weak_caching_keyed_supplier(
    std::function<std::shared_ptr<Value>(const Key &)> &&base_keyed_supplier);

}

#include <nosync/weak-caching-keyed-supplier-impl.h>

#endif
