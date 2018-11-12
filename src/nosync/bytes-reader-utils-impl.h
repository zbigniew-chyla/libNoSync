// This file is part of libnosync library. See LICENSE file for license details.
#ifndef NOSYNC__BYTES_READER_UTILS_IMPL_H
#define NOSYNC__BYTES_READER_UTILS_IMPL_H

#include <nosync/int-range.h>
#include <nosync/number-utils.h>
#include <nosync/raw-error-result.h>
#include <nosync/result-handler.h>
#include <nosync/time-utils.h>
#include <nosync/type-utils.h>
#include <string_view>
#include <type_traits>
#include <utility>


namespace nosync
{

namespace bytes_reader_utils_impl
{

template<typename T1 = void>
constexpr std::conditional_t<std::is_void_v<T1>, std::tuple<>, std::tuple<T1>> decode_be_bytes_to_numbers_impl([[maybe_unused]] std::string_view input)
{
    if constexpr (std::is_void_v<T1>) {
        return {};
    } else {
        return {decode_leading_be_bytes_to_number<T1>(input)};
    }
}


template<typename T1, typename T2, typename ...TT>
constexpr std::tuple<T1, T2, TT...> decode_be_bytes_to_numbers_impl(std::string_view input)
{
    return std::tuple_cat(
        decode_be_bytes_to_numbers_impl<T1>(input),
        decode_be_bytes_to_numbers_impl<T2, TT...>(input.substr(sizeof(T1), sizeof_sum<T2, TT...>)));
}


template<typename ...T>
constexpr std::tuple<T...> decode_be_bytes_to_numbers(std::string_view input)
{
    return decode_be_bytes_to_numbers_impl<T...>(input);
}

}


template<typename ...T, typename F>
void read_be_numbers_fully(std::shared_ptr<bytes_reader> &&reader, F &&res_handler)
{
    read_bytes_fully(
        std::move(reader), sizeof_sum<T...>,
        [res_handler = std::function<void(result<std::tuple<T...>>)>(std::forward<F>(res_handler))](auto read_res) {
            res_handler(
                read_res.is_ok()
                    ? make_ok_result(bytes_reader_utils_impl::decode_be_bytes_to_numbers<T...>(std::move(read_res.get_value())))
                    : raw_error_result(read_res));
        });
}


template<typename T>
void read_be_numbers_fully(
    std::shared_ptr<bytes_reader> reader, std::size_t count,
    result_handler<std::vector<T>> &&res_handler)
{
    read_bytes_fully(
        std::move(reader), sizeof(T) * count,
        transform_result_handler<std::string>(
            std::move(res_handler),
            [count](auto read_data) {
                std::string_view bytes_view(read_data);
                std::vector<T> res_numbers;
                res_numbers.reserve(count);
                for (auto i : int_range(count)) {
                    res_numbers.push_back(decode_leading_be_bytes_to_number<T>(bytes_view.substr(i * sizeof(T), sizeof(T))));
                }
                return make_ok_result(std::move(res_numbers));
            }));
}


template<typename T>
void read_be_number_fully(std::shared_ptr<bytes_reader> reader, result_handler<T> &&res_handler)
{
    read_bytes_fully(
        std::move(reader), sizeof(T),
        [res_handler = std::move(res_handler)](auto read_res) {
            res_handler(
                read_res.is_ok()
                    ? make_ok_result(decode_leading_be_bytes_to_number<T>(read_res.get_value()))
                    : raw_error_result(read_res));
        });
}


template<typename Res>
void process_read_bytes_with_timeout(
    event_loop &evloop, std::shared_ptr<bytes_reader> &&reader,
    std::chrono::nanoseconds timeout,
    std::function<std::optional<Res>(char)> &&bytes_processor,
    result_handler<Res> &&res_handler)
{
    const auto end_time = time_point_sat_add(evloop.get_etime(), timeout);
    reader->read_some_bytes(
        1, timeout,
        [&evloop, reader, bytes_processor = std::move(bytes_processor), end_time, res_handler = std::move(res_handler)](auto read_res) mutable {
            if (!read_res.is_ok()) {
                res_handler(std::move(read_res));
            } else {
                std::optional<Res> res;
                const auto &read_bytes = read_res.get_value();
                if (!read_bytes.empty()) {
                    res = bytes_processor(read_bytes.front());
                }

                if (res) {
                    res_handler(make_ok_result(std::move(*res)));
                } else {
                    process_read_bytes_with_timeout(
                        evloop, std::move(reader), end_time - evloop.get_etime(),
                        std::move(bytes_processor), std::move(res_handler));
                }
            }
        });
}


template<typename F>
void invoke_later_via_bytes_reader(bytes_reader &reader, F &&task)
{
    reader.read_some_bytes(
        0U, std::chrono::nanoseconds::max(),
        [task = std::forward<F>(task)](auto) mutable {
            task();
        });
}


template<typename Res>
void invoke_result_handler_later_via_bytes_reader(
    bytes_reader &reader, result_handler<Res> &&res_handler, result<Res> &&res)
{
    invoke_later_via_bytes_reader(
        reader,
        [res_handler = std::move(res_handler), res = std::move(res)]() mutable {
            res_handler(std::move(res));
        });
}


template<typename Res, typename ResultLike>
void invoke_result_handler_later_via_bytes_reader(
    bytes_reader &reader, result_handler<Res> &&res_handler, const ResultLike &res)
{
    invoke_result_handler_later_via_bytes_reader(reader, std::move(res_handler), result<Res>(res));
}

}

#endif /* NOSYNC__BYTES_READER_UTILS_IMPL_H */
