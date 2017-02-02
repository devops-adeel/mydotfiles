//
// Copyright (C) 2011-15 DyND Developers
// BSD 2-Clause License, see LICENSE.txt
//

#pragma once

#include <dynd/kernels/apply.hpp>
#include <dynd/kernels/base_strided_kernel.hpp>

namespace dynd {
namespace nd {
  namespace functional {
    namespace detail {

      template <typename T, typename mem_func_type, typename R, typename A, typename I, typename K, typename J>
      struct apply_member_function_kernel;

      template <typename T, typename mem_func_type, typename R, typename... A, size_t... I, typename... K, size_t... J>
      struct apply_member_function_kernel<T *, mem_func_type, R, type_sequence<A...>, index_sequence<I...>,
                                          type_sequence<K...>, index_sequence<J...>>
          : base_strided_kernel<
                apply_member_function_kernel<T *, mem_func_type, R, type_sequence<A...>, index_sequence<I...>,
                                             type_sequence<K...>, index_sequence<J...>>,
                sizeof...(A)>,
            apply_args<type_sequence<A...>, index_sequence<I...>>,
            apply_kwds<type_sequence<K...>, index_sequence<J...>> {
        typedef apply_args<type_sequence<A...>, index_sequence<I...>> args_type;
        typedef apply_kwds<type_sequence<K...>, index_sequence<J...>> kwds_type;
        typedef std::pair<T *, mem_func_type> data_type;

        T *obj;
        mem_func_type mem_func;

        apply_member_function_kernel(T *obj, mem_func_type mem_func, args_type args, kwds_type kwds)
            : args_type(args), kwds_type(kwds), obj(obj), mem_func(mem_func)
        {
        }

        void single(char *dst, char *const *DYND_IGNORE_UNUSED(src))
        {
          *reinterpret_cast<R *>(dst) = (obj->*mem_func)(apply_arg<A, I>::get(src[I])..., apply_kwd<K, J>::get()...);
        }

        static void instantiate(char *static_data, char *DYND_UNUSED(data), kernel_builder *ckb,
                                const ndt::type &DYND_UNUSED(dst_tp), const char *DYND_UNUSED(dst_arrmeta),
                                intptr_t DYND_UNUSED(nsrc), const ndt::type *src_tp, const char *const *src_arrmeta,
                                kernel_request_t kernreq, intptr_t nkwd, const nd::array *kwds,
                                const std::map<std::string, ndt::type> &DYND_UNUSED(tp_vars))
        {
          ckb->emplace_back<apply_member_function_kernel>(
              kernreq, reinterpret_cast<data_type *>(static_data)->first,
              dynd::detail::make_value_wrapper(reinterpret_cast<data_type *>(static_data)->second),
              args_type(src_tp, src_arrmeta, kwds), kwds_type(nkwd, kwds));
        }
      };

      template <typename T, typename mem_func_type, typename... A, size_t... I, typename... K, size_t... J>
      struct apply_member_function_kernel<T *, mem_func_type, void, type_sequence<A...>, index_sequence<I...>,
                                          type_sequence<K...>, index_sequence<J...>>
          : base_strided_kernel<
                apply_member_function_kernel<T *, mem_func_type, void, type_sequence<A...>, index_sequence<I...>,
                                             type_sequence<K...>, index_sequence<J...>>,
                sizeof...(A)>,
            apply_args<type_sequence<A...>, index_sequence<I...>>,
            apply_kwds<type_sequence<K...>, index_sequence<J...>> {
        typedef apply_args<type_sequence<A...>, index_sequence<I...>> args_type;
        typedef apply_kwds<type_sequence<K...>, index_sequence<J...>> kwds_type;
        typedef std::pair<T *, mem_func_type> data_type;

        T *obj;
        mem_func_type mem_func;

        apply_member_function_kernel(T *obj, mem_func_type mem_func, args_type args, kwds_type kwds)
            : args_type(args), kwds_type(kwds), obj(obj), mem_func(mem_func)
        {
        }

        void single(char *DYND_UNUSED(dst), char *const *DYND_IGNORE_UNUSED(src))
        {
          (obj->*mem_func)(apply_arg<A, I>::get(src[I])..., apply_kwd<K, J>::get()...);
        }

        static void instantiate(char *static_data, char *DYND_UNUSED(data), kernel_builder *ckb,
                                const ndt::type &DYND_UNUSED(dst_tp), const char *DYND_UNUSED(dst_arrmeta),
                                intptr_t DYND_UNUSED(nsrc), const ndt::type *src_tp, const char *const *src_arrmeta,
                                kernel_request_t kernreq, intptr_t nkwd, const nd::array *kwds,
                                const std::map<std::string, ndt::type> &DYND_UNUSED(tp_vars))
        {
          ckb->emplace_back<apply_member_function_kernel>(
              kernreq, reinterpret_cast<data_type *>(static_data)->first,
              dynd::detail::make_value_wrapper(reinterpret_cast<data_type *>(static_data)->second),
              args_type(src_tp, src_arrmeta, kwds), kwds_type(nkwd, kwds));
        }
      };

    } // namespace dynd::nd::functional::detail

    template <typename T, typename mem_func_type, int N>
    using apply_member_function_kernel =
        detail::apply_member_function_kernel<T, mem_func_type, typename return_of<mem_func_type>::type,
                                             as_apply_arg_sequence<mem_func_type, N>, make_index_sequence<N>,
                                             as_apply_kwd_sequence<mem_func_type, N>,
                                             make_index_sequence<arity_of<mem_func_type>::value - N>>;

  } // namespace dynd::nd::functional
} // namespace dynd::nd
} // namespace dynd