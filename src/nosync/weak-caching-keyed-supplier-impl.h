// This file is part of libnosync library. See LICENSE file for license details.
#ifndef NOSYNC__WEAK_CACHING_KEYED_SUPPLIER_IMPL_H
#define NOSYNC__WEAK_CACHING_KEYED_SUPPLIER_IMPL_H

#include <map>
#include <tuple>
#include <utility>


namespace nosync
{

namespace weak_caching_keyed_supplier_impl
{

template<typename Key, typename Value>
class weak_cache : public std::enable_shared_from_this<weak_cache<Key, Value>>
{
public:
    explicit weak_cache(std::function<std::shared_ptr<Value>(const Key &)> &&base_keyed_supplier);

    std::shared_ptr<Value> get_value_for_key(const Key &key);
    void remove_cached_value(typename std::map<Key, std::weak_ptr<Value>>::iterator cached_value_iter);

private:
    using std::enable_shared_from_this<weak_cache<Key, Value>>::shared_from_this;

    std::function<std::shared_ptr<Value>(const Key &)> base_keyed_supplier;
    std::map<Key, std::weak_ptr<Value>> cached_values;
};


template<typename Key, typename Value>
class cached_value_owner
{
public:
    cached_value_owner(
        std::weak_ptr<weak_cache<Key, Value>> cache_wptr, std::shared_ptr<Value> &&cached_value,
        typename std::map<Key, std::weak_ptr<Value>>::iterator cached_value_iter);
    ~cached_value_owner();

    cached_value_owner(const cached_value_owner &other) = delete;
    cached_value_owner &operator=(const cached_value_owner &other) = delete;

private:
    std::weak_ptr<weak_cache<Key, Value>> cache_wptr;
    std::shared_ptr<Value> cached_value;
    typename std::map<Key, std::weak_ptr<Value>>::iterator cached_value_iter;
};


template<typename Key, typename Value>
weak_cache<Key, Value>::weak_cache(std::function<std::shared_ptr<Value>(const Key &)> &&base_keyed_supplier)
    : base_keyed_supplier(std::move(base_keyed_supplier)), cached_values()
{
}


template<typename Key, typename Value>
std::shared_ptr<Value> weak_cache<Key, Value>::get_value_for_key(const Key &key)
{
    auto found_iter = cached_values.find(key);
    if (found_iter != cached_values.end()) {
        return found_iter->second.lock();
    }

    std::shared_ptr<Value> value;
    auto orig_value = base_keyed_supplier(key);
    if (orig_value) {
        typename std::map<Key, std::weak_ptr<Value>>::iterator cached_value_iter;
        std::tie(cached_value_iter, std::ignore) = cached_values.emplace(key, std::weak_ptr<Value>());
        auto orig_value_raw_ptr = orig_value.get();
        value = std::shared_ptr<Value>(
            std::make_shared<cached_value_owner<Key, Value>>(shared_from_this(), std::move(orig_value), cached_value_iter),
            orig_value_raw_ptr);
        cached_value_iter->second = value;
    } else {
        value = std::move(orig_value);
    }

    return value;
}


template<typename Key, typename Value>
void weak_cache<Key, Value>::remove_cached_value(typename std::map<Key, std::weak_ptr<Value>>::iterator cached_value_iter)
{
    cached_values.erase(cached_value_iter);
}


template<typename Key, typename Value>
cached_value_owner<Key, Value>::cached_value_owner(
    std::weak_ptr<weak_cache<Key, Value>> cache_wptr, std::shared_ptr<Value> &&cached_value,
    typename std::map<Key, std::weak_ptr<Value>>::iterator cached_value_iter)
    : cache_wptr(std::move(cache_wptr)), cached_value(std::move(cached_value)), cached_value_iter(cached_value_iter)
{
}


template<typename Key, typename Value>
cached_value_owner<Key, Value>::~cached_value_owner()
{
    auto cache_ptr = cache_wptr.lock();
    if (cache_ptr) {
        cache_ptr->remove_cached_value(cached_value_iter);
    }
}

}


template<typename Key, typename Value>
std::function<std::shared_ptr<Value>(const Key &)> make_weak_caching_keyed_supplier(
    std::function<std::shared_ptr<Value>(const Key &)> &&base_keyed_supplier)
{
    using namespace weak_caching_keyed_supplier_impl;

    return [cache = std::make_shared<weak_cache<Key, Value>>(std::move(base_keyed_supplier))](const Key &key) {
        return cache->get_value_for_key(key);
    };
}

}

#endif
