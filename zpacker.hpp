#pragma once

#include <array>
#include <tuple>
#include <iterator>
#include <algorithm>
#include <variant>
#include <vector>
#include <numeric>

namespace zpacker
{
    template <class...>
    inline constexpr bool Always_false = false;

    constexpr std::uint16_t VERSION_MAJOR = 0x0;
    constexpr std::uint16_t VERSION_MINOR = 0x1;

    constexpr std::uint16_t make_version(std::uint16_t major, std::uint16_t minor)
    {
        return major << 4 | minor;
    }

    constexpr std::uint16_t VERSION = make_version(VERSION_MAJOR, VERSION_MINOR);

    /* check if a type is a specialization of a template with single type and extract the single type of template */
    template <typename _Type, template <class...> typename _Template>
    struct is_specialize_of : std::false_type
    {
        using type = std::void_t<>;
    };

    template <template <class...> typename _Template, typename _Type>
    struct is_specialize_of<_Template<_Type>, _Template> : std::true_type
    {
        using type = _Type;
    };

    /* check if a type is a specialization of a template with sequence of types and extract the type list */
    template <typename _Type, template <class...> typename _Template>
    struct is_specialize_of_ex : std::false_type
    {
        using type = std::void_t<>;
    };

    template <template <class...> typename _Template, typename... _Types>
    struct is_specialize_of_ex<_Template<_Types...>, _Template> : std::true_type
    {
        using type = std::tuple<_Types...>;
    };

    /* check if a type is a specialization of a template type */
    template <typename _Type, template <class...> typename _Template>
    inline constexpr bool is_specialize_of_v = false;

    template <template <class...> typename _Template, typename... _Types>
    inline constexpr bool is_specialize_of_v<_Template<_Types...>, _Template> = true;

    class bytes_reader;
    class bytes_writer;

    class bytes_reader_bounded;
    class bytes_writer_bounded;

    namespace detail
    {
        template <class _Ty>
        auto has_begin_end_impl(int) -> decltype(std::declval<_Ty>().begin(), std::declval<_Ty>().end(), std::true_type{});

        template <class _Ty>
        std::false_type has_begin_end_impl(...);

        template <class _Ty, class _Vty>
        auto has_push_back_impl(int) -> decltype(std::declval<_Ty>().push_back(std::declval<_Vty>()), std::true_type{});

        template <class _Ty, class _Vty>
        std::false_type has_push_back_impl(...);

        template <class _Ty>
        auto has_push_back_impl_ex(int) -> decltype(std::declval<_Ty>().push_back(std::declval<typename _Ty::value_type>()), std::true_type{});

        template <class _Ty>
        std::false_type has_push_back_impl_ex(...);

        template <class _Ty>
        auto has_push_front(int) -> decltype(std::declval<_Ty>().push_front(std::declval<typename _Ty::value_type>()), std::true_type{});

        template <class _Ty>
        std::false_type has_push_front(...);

        // MSVC STL Specific
        // template <class _Ty>
        // auto has_iterator_impl(int) -> decltype(std::declval<std::iterator_traits<typename _Ty::const_iterator>::iterator_category>(),
        //                                         std::declval<typename _Ty::iterator>(), std::true_type{});

        template <class _Ty>
        auto has_iterator_impl(int) -> decltype(std::declval<typename _Ty::const_iterator>(), std::declval<typename _Ty::iterator>(), std::true_type{});

        template <class _Ty>
        std::false_type has_iterator_impl(...);

        template <class _Ty>
        auto has_insert_impl(int) -> decltype(std::declval<_Ty>().insert(std::declval<typename _Ty::value_type>()), std::true_type{});

        template <class _Ty>
        std::false_type has_insert_impl(...);

        template <class _Ty>
        auto has_size_impl(int) -> decltype(std::declval<_Ty>().size(), std::true_type{});

        template <class _Ty>
        std::false_type has_size_impl(...);

        template <class _Ty>
        auto has_reserve_impl(int) -> decltype(std::declval<_Ty>().reserve(0), std::true_type{});

        template <class _Ty>
        std::false_type has_reserve_impl(...);

        template <class _Ty>
        auto has_serialize1_impl(int) -> decltype(std::declval<_Ty>().serialize(std::declval<std::add_lvalue_reference_t<bytes_writer>>()), std::true_type{});

        template <class _Ty>
        std::false_type has_serialize1_impl(...);

        template <class _Ty>
        auto has_serialize2_impl(int) -> decltype(std::declval<_Ty>().serialize(std::declval<std::add_lvalue_reference_t<bytes_writer_bounded>>()), std::true_type{});

        template <class _Ty>
        std::false_type has_serialize2_impl(...);

        template <class _Ty>
        auto has_deserialize1_impl(int) -> decltype(_Ty::deserialize(std::declval<std::add_lvalue_reference_t<bytes_reader>>()), std::true_type{});

        template <class _Ty>
        std::false_type has_deserialize1_impl(...);

        template <class _Ty>
        auto has_deserialize2_impl(int) -> decltype(_Ty::deserialize(std::declval<std::add_lvalue_reference_t<bytes_reader_bounded>>()), std::true_type{});

        template <class _Ty>
        std::false_type has_deserialize2_impl(...);

        template <class _Ty>
        auto has_get_size_impl(int) -> decltype(std::declval<std::add_const_t<std::add_lvalue_reference_t<_Ty>>>().get_size(), std::true_type{});

        template <class _Ty>
        std::false_type has_get_size_impl(...);

        template <class _Ty>
        auto has_value_type_impl(int) -> decltype(std::declval<typename _Ty::value_type>(), std::true_type{});

        template <class _Ty>
        std::false_type has_value_type_impl(...);

        template <class _Ty, class _Vty>
        auto is_reader_impl(int) -> decltype(std::declval<_Ty>().template can_read<int>(),
                                             std::declval<_Ty>().template read<_Vty>(),
                                             std::declval<_Ty>() >> std::declval<std::add_lvalue_reference_t<_Vty>>(), std::true_type{});

        template <class _Ty, class _Vty>
        std::false_type is_reader_impl(...);

        template <class _Ty, class _Vty>
        auto is_writer_impl(int) -> decltype(std::declval<_Ty>().template can_write<int>(),
                                             std::declval<_Ty>().write(std::declval<_Vty>()),
                                             std::declval<_Ty>() << std::declval<_Vty>(), std::true_type{});

        template <class _Ty, class _Vty>
        std::false_type is_writer_impl(...);
    }

    template <class _Ty>
    using has_begin_end = decltype(detail::has_begin_end_impl<_Ty>(0));

    template <class _Ty>
    constexpr bool has_begin_end_v = has_begin_end<_Ty>::value;

    template <class _Ty>
    using has_iterator = decltype(detail::has_iterator_impl<_Ty>(0));

    template <class _Ty>
    constexpr bool has_iterator_v = has_iterator<_Ty>::value;

    template <class _Ty, class _Vty>
    using has_push_back = decltype(detail::has_push_back_impl<_Ty, _Vty>(0));

    template <class _Ty, class _Vty>
    constexpr bool has_push_back_v = has_push_back<_Ty, _Vty>::value;

    template <class _Ty>
    using has_push_back_ex = decltype(detail::has_push_back_impl_ex<_Ty>(0));

    template <class _Ty>
    constexpr bool has_push_back_ex_v = has_push_back_ex<_Ty>::value;

    template <class _Ty>
    using has_insert = decltype(detail::has_insert_impl<_Ty>(0));

    template <class _Ty>
    using has_push_front = decltype(detail::has_push_front<_Ty>(0));

    template <class _Ty>
    constexpr bool has_push_front_v = has_push_front<_Ty>::value;

    template <class _Ty>
    constexpr bool has_insert_v = has_insert<_Ty>::value;

    template <class _Ty>
    using has_size = decltype(detail::has_size_impl<_Ty>(0));

    template <class _Ty>
    constexpr bool has_size_v = has_size<_Ty>::value;

    template <class _Ty>
    using has_value_type = decltype(detail::has_value_type_impl<_Ty>(0));

    template <class _Ty>
    constexpr bool has_value_type_v = has_value_type<_Ty>::value;

    template <class _Ty>
    using has_reserve = decltype(detail::has_reserve_impl<_Ty>(0));

    template <class _Ty>
    constexpr bool has_reserve_v = has_reserve<_Ty>::value;

    template <class _Ty>
    using has_serialize_unbounded = decltype(detail::has_serialize1_impl<_Ty>(0));

    template <class _Ty>
    constexpr bool has_serialize_unbounded_v = has_serialize_unbounded<_Ty>::value;

    template <class _Ty>
    using has_serialize_bounded = decltype(detail::has_serialize2_impl<_Ty>(0));

    template <class _Ty>
    constexpr bool has_serialize_bounded_v = has_serialize_bounded<_Ty>::value;

    template <class _Ty>
    using has_serialize = std::disjunction<has_serialize_unbounded<_Ty>, has_serialize_bounded<_Ty>>;

    template <class _Ty>
    constexpr bool has_serialize_v = has_serialize<_Ty>::value;

    template <class _Ty>
    using has_deserialize_unbounded = decltype(detail::has_deserialize1_impl<_Ty>(0));

    template <class _Ty>
    constexpr bool has_deserialize_unbounded_v = has_deserialize_unbounded<_Ty>::value;

    template <class _Ty>
    using has_deserialize_bounded = decltype(detail::has_deserialize2_impl<_Ty>(0));

    template <class _Ty>
    constexpr bool has_deserialize_bounded_v = has_deserialize_bounded<_Ty>::value;

    template <class _Ty>
    using has_deserialize = std::disjunction<has_deserialize_unbounded<_Ty>, has_deserialize_bounded<_Ty>>;

    template <class _Ty>
    constexpr bool has_deserialize_v = has_deserialize<_Ty>::value;

    template <class _Ty>
    using has_get_size = decltype(detail::has_get_size_impl<_Ty>(0));

    template <class _Ty>
    constexpr bool has_get_size_v = has_get_size<_Ty>::value;

    template <class _Ty, class _Vty>
    using is_reader = decltype(detail::is_reader_impl<_Ty, _Vty>(0));

    template <class _Ty, class _Vty>
    constexpr bool is_reader_v = is_reader<_Ty, _Vty>::value;

    template <class _Ty, class _Vty>
    using is_writer = decltype(detail::is_writer_impl<_Ty, _Vty>(0));

    template <class _Ty, class _Vty>
    constexpr bool is_writer_v = is_writer<_Ty, _Vty>::value;

    template <class _Ty>
    using is_standard_container = std::conjunction<has_begin_end<_Ty>, has_iterator<_Ty>, has_size<_Ty>>;

    template <class _Ty>
    constexpr bool is_standard_container_v = is_standard_container<_Ty>::value;

    template <class _Ty>
    using is_sequence_container = std::conjunction<
        is_standard_container<_Ty>,
        std::disjunction<has_push_back_ex<std::remove_cv_t<_Ty>>,
                         has_push_front<std::remove_cv_t<_Ty>>>>;

    template <class _Ty>
    constexpr bool is_sequence_container_v = is_sequence_container<_Ty>::value;

    template <class _Ty>
    using is_associated_container = std::conjunction<is_standard_container<_Ty>, has_insert<std::remove_cv_t<_Ty>>>;

    template <class _Ty>
    constexpr bool is_associated_container_v = is_associated_container<_Ty>::value;

    template <class _Ty>
    using remove_cvref_t = std::remove_cv_t<std::remove_reference_t<_Ty>>;

    enum data_type
    {
        d_empty = 0,

        d_byte8,
        d_byte16,
        d_byte32,
        d_byte64,
        d_float32,
        d_float64,

        d_pod,

        d_pair,

        d_variant,

        d_tuple,

        d_seq_container,

        d_aso_container,

        d_custom
    };

#pragma warning(disable : 4702)
    template <class _Ty>
    constexpr data_type get_data_type()
    {
        constexpr size_t _Size = sizeof(_Ty);

        if constexpr (is_specialize_of_v<_Ty, std::pair>)
            return d_pair;
        else if constexpr (is_specialize_of_v<_Ty, std::variant>)
            return d_variant;
        else if constexpr (is_specialize_of_v<_Ty, std::tuple>)
            return d_tuple;
        else if constexpr (is_sequence_container_v<_Ty>)
            return d_seq_container;
        else if constexpr (is_associated_container_v<_Ty>)
            return d_aso_container;
        else if constexpr (std::is_trivially_copy_constructible_v<_Ty>)
        {
            if constexpr (std::is_floating_point_v<_Ty>)
            {
                if constexpr (_Size == 4)
                    return d_float32;
                else if constexpr (_Size == 8)
                    return d_float64;
            }
            else if constexpr (std::is_integral_v<_Ty>)
            {
                if constexpr (_Size == 1)
                    return d_byte8;
                else if constexpr (_Size == 2)
                    return d_byte16;
                else if constexpr (_Size == 4)
                    return d_byte32;
                else if constexpr (_Size == 8)
                    return d_byte64;
            }
        }

        return d_custom;
    }
#pragma warning(default : 4702)

#pragma pack(push, 1)
    struct data_header
    {
        uint8_t type;
        std::uint32_t length;

        void set_main_type(data_type dt)
        {
            this->type &= 0xf0;
            this->type |= static_cast<uint8_t>(dt);
        }

        void set_sub_type(data_type dt)
        {
            this->type &= 0x0f;
            this->type |= static_cast<uint8_t>(dt) << 4;
        }

        data_type get_main_type()
        {
            return static_cast<data_type>(this->type & 0x0f);
        }

        data_type get_sub_type()
        {
            return static_cast<data_type>(this->type >> 4);
        }

        template <class _Ty>
        bool is_subtype_compitable()
        {
            auto subdt = get_sub_type();
            constexpr auto dt = get_data_type<_Ty>();

#pragma warning(disable : 4127)

            if (dt < d_pod && subdt < d_pod)
            {
                return subdt >= dt;
            }
#pragma warning(default : 4127)

            return subdt == dt;
        }
    };

    struct packer_header
    {
        std::uint16_t version;

        union
        {
            uint8_t crc8;
            std::uint16_t crc16;
            std::uint32_t crc32;
        } crc;

        std::uint32_t length;

        void set_version(std::uint16_t ver)
        {
            version = ver;
        }

        void set_major_version(uint8_t major)
        {
            version &= 0x00ff;
            version |= (std::uint16_t)major << 8;
        }

        void set_minor_version(uint8_t minor)
        {
            version &= 0xff00;
            version |= (std::uint16_t)minor;
        }
    };
#pragma pack(pop)

    struct empty_checksum
    {
        std::uint32_t operator()(const uint8_t *data, size_t length) const
        {
            (void *)data;
            (void *)length;

            return 0;
        }
    };

    constexpr uint8_t polynomial_crc8 = 0x07;

    constexpr uint8_t entry_crc8(uint8_t i)
    {
        uint8_t crc = i;

        for (int j = 0; j < 8; ++j)
            crc = (crc & 0x80) ? (crc << 1) ^ polynomial_crc8 : (crc << 1);

        return crc;
    }

    constexpr auto generate_crc8_table()
    {
        std::array<uint8_t, 256> table = {};

        for (size_t i = 0; i < table.size(); ++i)
            table[i] = entry_crc8(static_cast<uint8_t>(i));

        return table;
    }

    static constexpr std::array<uint8_t, 256> CRC8_TABLE = generate_crc8_table();

    constexpr std::uint16_t polynomial_crc16 = 0x1021;

    constexpr std::uint16_t entry_crc16(uint8_t i)
    {
        std::uint16_t crc = static_cast<std::uint16_t>(i) << 8;

        for (int j = 0; j < 8; ++j)
            crc = (crc & 0x8000) ? (crc << 1) ^ polynomial_crc16 : (crc << 1);

        return crc;
    }

    constexpr auto generate_crc16_table()
    {
        std::array<std::uint16_t, 256> table = {};

        for (size_t i = 0; i < table.size(); ++i)
            table[i] = entry_crc16(static_cast<uint8_t>(i));

        return table;
    }

    static constexpr std::array<std::uint16_t, 256> CRC16_TABLE = generate_crc16_table();

    constexpr std::uint32_t _polynomial_crc32 = 0xEDB88320;

    constexpr std::uint32_t entry_crc32(std::uint32_t i)
    {
        std::uint32_t crc = i;

        for (int j = 0; j < 8; ++j)
            crc = (crc & 1) ? (crc >> 1) ^ _polynomial_crc32 : (crc >> 1);

        return crc;
    }

    constexpr auto generate_crc32_table()
    {
        std::array<std::uint32_t, 256> table = {};

        for (size_t i = 0; i < table.size(); ++i)
            table[i] = entry_crc32(static_cast<std::uint32_t>(i));

        return table;
    }

    static constexpr std::array<std::uint32_t, 256> CRC32_TABLE = generate_crc32_table();

    struct crc8_checksum
    {
        uint8_t operator()(const uint8_t *data, size_t length) const
        {
            uint8_t crc = 0x0;

            for (size_t i = 0; i < length; ++i)
                crc = CRC8_TABLE[crc ^ data[i]];

            return crc;
        }
    };

    struct crc16_checksum
    {
        std::uint16_t operator()(const uint8_t *data, size_t length) const
        {
            std::uint16_t crc = 0xFFFF;

            for (size_t i = 0; i < length; ++i)
                crc = (crc << 8) ^ CRC16_TABLE[(crc >> 8) ^ data[i]];

            return crc;
        }
    };

    struct crc32_checksum
    {
        std::uint32_t operator()(const uint8_t *data, size_t length) const
        {
            std::uint32_t crc = 0xFFFFFFFF;

            for (size_t i = 0; i < length; ++i)
                crc = (crc >> 8) ^ CRC32_TABLE[(crc ^ data[i]) & 0xFF];

            return ~crc;
        }
    };

    constexpr size_t _default_reserve_size = 4096;

    struct empty_encoder
    {
        std::vector<uint8_t> operator()(const void *input, size_t length) const
        {
            return std::vector<uint8_t>{(uint8_t *)input, (uint8_t *)input + length};
        }
    };

    struct empty_decoder
    {
        std::vector<uint8_t> operator()(const void *input, size_t length) const
        {
            return std::vector<uint8_t>{(uint8_t *)input, (uint8_t *)input + length};
        }
    };

    // forward declaration
    template <class _Ty, class _Writer>
    void serialize_object(_Writer &, const _Ty &);

    template <
        class _Ty,
        class _Reader,
        class>
    _Ty deserialize_object(_Reader &);

    class bytes_reader
    {
    public:
        bytes_reader(const std::vector<uint8_t> &data) : m_data(std::addressof(data)) {}

        template <class _Vty>
        _Vty read()
        {
            if constexpr (std::is_trivially_copyable_v<_Vty>)
            {
                if (!can_read<_Vty>())
                    return _Vty{};

                auto result = *reinterpret_cast<_Vty *>(const_cast<uint8_t *>(m_data->data()) + m_pos);

                m_pos += sizeof(_Vty);

                return result;
            }
            else
            {
                return deserialize_object<_Vty>(*this);
            }
        }

        template <class _Vty>
        bytes_reader &operator>>(_Vty &val)
        {
            val = this->read<_Vty>();

            return *this;
        }

        std::vector<uint8_t> read_bytes(size_t count)
        {
            auto available = (std::min)(count, m_data->size() - count);

            auto result = std::vector<uint8_t>{m_data->data() + m_pos, m_data->data() + m_pos + available};

            m_pos += available;

            return result;
        }

        template <class _Vty, std::enable_if_t<std::is_trivially_constructible_v<_Vty>, int> = 0>
        bool can_read() const
        {
            return remaining() >= sizeof(_Vty);
        }

        size_t remaining() const
        {
            return m_data->size() - m_pos;
        }

        /*
         * Return total bytes that has been read out for now
         * It also represent current buffer read position
         */
        size_t count() const
        {
            return m_pos;
        }

        void skip(size_t count)
        {
            if (remaining() >= count)
                m_pos += count;
        }

        void seek(size_t pos)
        {
            if (pos < m_data->size() - count())
                m_pos = pos;
        }

        void reset(const std::vector<uint8_t> *data)
        {
            m_pos = 0;
            m_data = data;
        }

    private:
        size_t m_pos{0};
        const std::vector<uint8_t> *m_data;
    };

    class bytes_reader_bounded
    {
    public:
        bytes_reader_bounded(const uint8_t *data, size_t length) : m_data(data), m_length(length) {}

        template <class _Vty>
        _Vty read()
        {
            if constexpr (std::is_trivially_copyable_v<_Vty>)
            {
                static_assert(std::is_default_constructible_v<_Vty>, "_Vty must be default constructible");

                if (!can_read<_Vty>())
                    return _Vty{};

                auto result = *reinterpret_cast<const _Vty *>(m_data + m_pos);

                m_pos += sizeof(_Vty);

                return result;
            }
            else
            {
                return deserialize_object<_Vty>(*this);
            }
        }

        template <class _Vty>
        bytes_reader_bounded &operator>>(_Vty &val)
        {
            val = this->read<_Vty>();

            return *this;
        }

        std::vector<uint8_t> read_bytes(size_t count)
        {
            auto available = (std::min)(count, m_length - count);

            auto result = std::vector<uint8_t>{m_data + m_pos, m_data + m_pos + available};

            m_pos += available;

            return result;
        }

        template <class _Vty, std::enable_if_t<std::is_trivially_copyable_v<_Vty>, int> = 0>
        bool can_read() const
        {
            return remaining() >= sizeof(_Vty);
        }

        size_t remaining() const
        {
            return m_length - m_pos;
        }

        void skip(size_t count)
        {
            if (remaining() >= count)
                m_pos += count;
        }

        size_t count() const
        {
            return m_pos;
        }

        void seek(size_t pos)
        {
            if (pos < m_length - count())
                m_pos = pos;
        }

        void reset(const uint8_t *data, size_t length)
        {
            m_pos = 0;
            m_data = data;
            m_length = length;
        }

    private:
        size_t m_pos{0};
        const uint8_t *m_data{nullptr};
        size_t m_length{0};
    };

    class bytes_writer
    {
    public:
        bytes_writer(std::vector<uint8_t> &data) : m_data(std::addressof(data)) {}

        template <class _Vty>
        void write(const _Vty &val)
        {
            if constexpr (std::is_trivially_copyable_v<_Vty>)
            {
                auto begin = (uint8_t *)std::addressof(val);

                m_data->insert(m_data->end(), begin, begin + sizeof(_Vty));
            }
            else
            {
                serialize_object(*this, val);
            }
        }

        void write(const std::vector<uint8_t> &data)
        {
            std::copy(data.begin(), data.end(), std::back_inserter(*m_data));
        }

        void write(const uint8_t *data, size_t length)
        {
            std::copy(data, data + length, std::back_inserter(*m_data));
        }

        template <class _Vty>
        bytes_writer &operator<<(const _Vty &val)
        {
            this->write(val);

            return *this;
        }

        void reset(std::vector<uint8_t> &data)
        {
            m_data = std::addressof(data);
        }

        template <class _Ty>
        constexpr bool can_write() const
        {
            return true;
        }

        /*
         * Get the total bytes written
         */
        size_t count() const
        {
            return m_data->size();
        }

    private:
        std::vector<uint8_t> *m_data;
    };

    class bytes_writer_bounded
    {
    public:
        bytes_writer_bounded(uint8_t *data, size_t length) : m_data(data), m_length(length) {}

        template <class _Vty>
        void write(const _Vty &val)
        {
            if constexpr (std::is_trivially_copyable_v<_Vty>)
            {
                if (can_write<_Vty>())
                {
                    *(_Vty *)(m_data + m_pos) = val;

                    m_pos += sizeof(_Vty);
                }
            }
            else
            {
                serialize_object(*this, val);
            }
        }

        void write(const uint8_t *data, size_t length)
        {
            auto _copy_len = (std::min)(length, m_length - m_pos);

            if (_copy_len > 0)
            {
                memcpy(m_data + m_pos, data, _copy_len);

                m_pos += _copy_len;
            }
        }

        template <class _Vty>
        bytes_writer_bounded &operator<<(const _Vty &val)
        {
            this->write(val);

            return *this;
        }

        template <class _Vty, std::enable_if_t<std::is_trivially_copyable_v<_Vty>, int> = 0>
        bool can_write() const
        {
            return remaining() >= sizeof(_Vty);
        }

        void reset(uint8_t *data, size_t length)
        {
            m_pos = 0;
            m_data = data;
            m_length = length;
        }

        /*
         * Get the total bytes written
         */
        size_t count() const
        {
            return m_pos;
        }

        size_t remaining() const
        {
            return m_length - m_pos;
        }

    private:
        uint8_t *m_data{nullptr};
        size_t m_pos{0};
        size_t m_length{0};
    };

    template <class _Ty>
    constexpr size_t get_size(const _Ty &);

    namespace detail
    {
        template <class _Variant, size_t... _Indices>
        constexpr size_t get_variant_size_impl(const _Variant &variant, std::index_sequence<_Indices...>)
        {
            using _Get_variant_size_t = size_t (*)(const _Variant &);

            constexpr _Get_variant_size_t _table[] =
                {
                    [](const _Variant &variant) -> size_t
                    {
                        using value_type = std::variant_alternative_t<_Indices, _Variant>;
                        return get_size<value_type>(std::get<value_type>(variant));
                    }...};

            return _table[variant.index()](variant);
        }

        template <class _Tuple, size_t... _Indices>
        constexpr size_t get_tuple_size_impl(const _Tuple &tuple, std::index_sequence<_Indices...>)
        {
            const size_t _sizes[] = {get_size<std::tuple_element_t<_Indices, _Tuple>>(std::get<_Indices>(tuple))...};

            return std::accumulate(_sizes, _sizes + std::tuple_size_v<_Tuple>, (size_t)0);
        }

        template <class _Tuple, class _Writer, size_t... _Indices>
        void serialize_tuple_impl(_Writer &writer, const _Tuple &tuple, std::index_sequence<_Indices...>)
        {
            (writer << ... << std::get<_Indices>(tuple));
        }

        template <class _Variant, class _Reader, size_t... _Indices>
        _Variant deserialize_variant_impl(_Reader &reader, uint32_t index, std::index_sequence<_Indices...>)
        {
            using _Variant_deserializer_t = _Variant (*)(_Reader &);

            constexpr _Variant_deserializer_t _table[] =
                {
                    [](_Reader &reader) -> _Variant
                    {
                        using value_type = std::variant_alternative_t<_Indices, _Variant>;

                        return _Variant{reader.template read<value_type>()};
                    }...};

            return _table[index](reader);
        }

        template <class _Tuple, class _Reader, size_t... _Indices>
        _Tuple deserialize_tuple_impl(_Reader &reader, std::index_sequence<_Indices...>)
        {
            _Tuple __t{};

            (reader >> ... >> std::get<_Indices>(__t));

            return __t;
        }
    }

    template <class _Ty>
    constexpr void get_object_size(const _Ty &object, size_t &size)
    {
        constexpr size_t header_size = sizeof(data_header);

        static_assert(!std::is_pointer_v<remove_cvref_t<_Ty>>, "value_type in container _Ty to be serialized can not be pointer type");

        if constexpr (has_get_size_v<_Ty>)
        {
            size += object.get_size();
        }
        else if constexpr (is_specialize_of_v<remove_cvref_t<_Ty>, std::pair>)
        {
            size += sizeof(data_header);

            get_object_size(object.first, size);
            get_object_size(object.second, size);
        }
        else if constexpr (is_specialize_of_v<remove_cvref_t<_Ty>, std::variant>)
        {
            using _Variant = remove_cvref_t<_Ty>;

            size += sizeof(data_header);

            size += detail::get_variant_size_impl(object, std::make_index_sequence<std::variant_size_v<_Variant>>{});
        }
        else if constexpr (is_specialize_of_v<remove_cvref_t<_Ty>, std::tuple>)
        {
            using _Tuple = remove_cvref_t<_Ty>;

            size += sizeof(data_header);

            size += detail::get_tuple_size_impl(object, std::make_index_sequence<std::tuple_size_v<_Tuple>>{});
        }
        else if constexpr (is_standard_container_v<remove_cvref_t<_Ty>>)
        {
            using value_type = typename remove_cvref_t<_Ty>::value_type;

            size += header_size;

            /* with this constexpr, compiler can generate more efficient code */
            if constexpr (get_data_type<value_type>() < d_pod)
            {
                size += sizeof(value_type) * object.size();
            }
            else
            {
                std::for_each(object.begin(), object.end(), [&size](auto &v)
                              { get_object_size(v, size); });
            }
        }
        else if constexpr (has_iterator_v<remove_cvref_t<_Ty>> && has_value_type_v<remove_cvref_t<_Ty>>)
        {
            using value_type = typename remove_cvref_t<_Ty>::value_type;

            size += header_size;

            if constexpr (get_data_type<value_type>() < d_pod)
            {
                size += sizeof(value_type) * object.size();
            }
            else
            {
                std::for_each(object.begin(), object.end(), [&size](auto &v)
                              { get_object_size(v, size); });
            }
        }
        else if constexpr (std::is_trivially_copyable_v<remove_cvref_t<_Ty>>)
        {
            if constexpr (std::is_compound_v<_Ty>)
            {
                size += header_size;
            }

            size += sizeof(_Ty);
        }
        else
        {
            static_assert(
                Always_false<_Ty>,
                "_Ty must implement \"size_t _Ty::get_size() const\"");
        }
    }

    /*
     * Calculate the memory space size needed for serialization / deserialization of the object
     */
    template <class _Ty>
    constexpr size_t get_size(const _Ty &object)
    {
        size_t result{};

        get_object_size(object, result);

        return result;
    }

    /*
     * Serialize a object to binary format
     */
    template <class _Ty, class _Writer = bytes_writer>
    void serialize_object(_Writer &writer, const _Ty &object)
    {
        static_assert(!std::is_pointer_v<remove_cvref_t<_Ty>>, "value_type in container _Ty to be serialized can not be pointer type");

        if constexpr (has_serialize_v<_Ty>)
        {
            object.serialize(writer);
        }
        else if constexpr (is_specialize_of_v<remove_cvref_t<_Ty>, std::pair>)
        {
            data_header _header{d_pair, 2};

            writer << _header;

            using _first_type = typename remove_cvref_t<_Ty>::first_type;
            using _second_type = typename remove_cvref_t<_Ty>::second_type;

            writer << object.first;
            writer << object.second;
        }
        else if constexpr (is_specialize_of_v<remove_cvref_t<_Ty>, std::variant>)
        {
            using _Variant = remove_cvref_t<_Ty>;

            data_header _header{d_variant, std::variant_size_v<_Variant>};

            std::visit([&_header, &writer, &object](auto &&val)
                       {
				using value_type = std::decay_t<decltype(val)>;

				_header.set_sub_type(get_data_type<value_type>());

				writer << _header << static_cast<uint32_t>(object.index()) << val; }, object);
        }
        else if constexpr (is_specialize_of_v<remove_cvref_t<_Ty>, std::tuple>)
        {
            using _Tuple = remove_cvref_t<_Ty>;

            writer << data_header{d_tuple, std::tuple_size_v<_Tuple>};

            detail::serialize_tuple_impl(writer, object, std::make_index_sequence<std::tuple_size_v<_Tuple>>{});
        }
        else if constexpr (is_standard_container_v<remove_cvref_t<_Ty>>)
        {
            using container_type = remove_cvref_t<_Ty>;
            using value_type = typename container_type::value_type;

            data_header _header{};

            if constexpr (is_sequence_container_v<container_type>)
            {
                _header.set_main_type(d_seq_container);
                _header.set_sub_type(get_data_type<value_type>());
            }
            else if constexpr (is_associated_container_v<container_type>)
            {
                _header.set_main_type(d_aso_container);
                _header.set_sub_type(get_data_type<value_type>());
            }
            /* std::array, std::initializer_list, etc. */
            else
            {
                _header.set_main_type(d_seq_container);
                _header.set_sub_type(get_data_type<value_type>());
            }

            _header.length = static_cast<std::uint32_t>(object.size());

            writer << _header;

            std::for_each(object.begin(), object.end(), [&writer](auto &v)
                          { writer << v; });
        }
        /* std::forward_list goes here */
        else if constexpr (has_iterator_v<remove_cvref_t<_Ty>> && has_value_type_v<remove_cvref_t<_Ty>>)
        {
            using container_type = remove_cvref_t<_Ty>;
            using value_type = typename container_type::value_type;

            data_header _header{};

            _header.set_main_type(d_seq_container);
            _header.set_sub_type(get_data_type<value_type>());

            if constexpr (has_size_v<container_type>)
            {
                _header.length = static_cast<std::uint32_t>(object.size());

                writer << _header;

                std::for_each(object.begin(), object.end(), [&writer](auto &v)
                              { writer << v; });
            }
            else
            {
                std::uint32_t _size{0};
                std::vector<uint8_t> _partial;

                _partial.reserve(_default_reserve_size);

                bytes_writer _writer{_partial};

                std::for_each(object.begin(), object.end(), [&_writer, &_partial, &_size](auto &v)
                              {
					_writer << v;
					++_size; });

                _header.length = _size;

                _partial.shrink_to_fit();

                writer << _header;

                writer.write(_partial);
            }
        }
        else if constexpr (std::is_trivially_copyable_v<remove_cvref_t<_Ty>>)
        {
            if constexpr (std::is_compound_v<_Ty>)
            {
                writer << data_header{d_pod, static_cast<std::uint32_t>(sizeof(_Ty))};
            }

            writer << object;
        }
        else
        {
            static_assert(
                Always_false<_Ty>,
                "_Ty to serialize must be either of a standard STL container type, custom containers that implement standard iterator, "
                "POD type or custom types that implement serialize() method");
        }
    }

    /*
     * Deserialize a object from binary format
     */
    template <
        class _Ty,
        class _Reader = bytes_reader,
        std::enable_if_t<std::is_default_constructible_v<_Ty>, int> = 0>
    _Ty deserialize_object(_Reader &reader)
    {
        static_assert(!std::is_pointer_v<remove_cvref_t<_Ty>>, "value_type in container _Ty to be deserialized can not be pointer type");

        if constexpr (has_deserialize_v<_Ty>)
        {
            return _Ty::deserialize(reader);
        }
        else if constexpr (is_specialize_of_v<_Ty, std::pair>)
        {
            using first_type = typename _Ty::first_type;
            using second_type = typename _Ty::second_type;

            auto _header = reader.template read<data_header>();

            // runtime check
            if (_header.length != 2 || _header.get_main_type() != d_pair)
                return _Ty{};

            return _Ty{reader.template read<first_type>(), reader.template read<second_type>()};
        }
        else if constexpr (is_specialize_of_v<_Ty, std::variant>)
        {
            using _Variant = _Ty;

            auto _header = reader.template read<data_header>();

            if (_header.length != std::variant_size_v<_Variant>)
                return _Variant{};

            auto _index = reader.template read<uint32_t>();

            if (_index > _header.length)
                return _Variant{};

            return detail::deserialize_variant_impl<_Variant>(reader, _index, std::make_index_sequence<std::variant_size_v<_Variant>>{});
        }
        else if constexpr (is_specialize_of_v<_Ty, std::tuple>)
        {
            using _Tuple = _Ty;

            auto _header = reader.template read<data_header>();

            if (_header.length != std::tuple_size_v<_Tuple>)
                return _Ty{};

            return detail::deserialize_tuple_impl<_Tuple>(reader, std::make_index_sequence<std::tuple_size_v<_Tuple>>{});
        }
        else if constexpr (is_standard_container_v<_Ty>)
        {
            using value_type = typename _Ty::value_type;

            auto _header = reader.template read<data_header>();

            std::remove_cv_t<_Ty> container{};

            if constexpr (is_sequence_container_v<_Ty>)
            {
                // runtime check
                if (_header.get_main_type() == d_seq_container &&
                    _header.template is_subtype_compitable<value_type>())
                {
                    for (std::uint32_t i = 0; i < _header.length; i++)
                    {
                        container.push_back(reader.template read<value_type>());
                    }
                }
            }
            else if constexpr (is_associated_container_v<_Ty>)
            {
                // runtime check
                if (_header.get_main_type() == d_aso_container &&
                    _header.template is_subtype_compitable<value_type>())
                {
                    for (std::uint32_t i = 0; i < _header.length; i++)
                    {
                        container.insert(reader.template read<value_type>());
                    }
                }
            }

            return container;
        }
        else if constexpr (std::is_trivially_copyable_v<_Ty>)
        {
            if constexpr (std::is_compound_v<_Ty>)
            {
                auto _header = reader.template read<data_header>();

                // runtime check
                if (_header.length < sizeof(_Ty))
                    return _Ty{};
            }

            return reader.template read<_Ty>();
        }
        else
        {
            static_assert(
                Always_false<_Ty>,
                "_Ty to deserialize must be either of a standard STL container type, custom containers that implement standard iterator, "
                "POD type, arithmetic type or custom types that implement deserialize() method");
        }
    }

    template <
        class _Ty,
        class _CheckSum = empty_checksum>
    std::vector<uint8_t> serialize(const _Ty &value, _CheckSum checksum = empty_checksum{})
    {
        std::vector<uint8_t> data{};
        std::vector<uint8_t> result{};

        data.reserve(_default_reserve_size);

        bytes_writer writer{data};

        // serialization
        serialize_object(writer, value);

        data.shrink_to_fit();

        result.reserve(data.size() + sizeof(packer_header));

        // insert packer header
        packer_header ph{};

        ph.set_version(VERSION);

        ph.crc.crc32 = checksum(data.data(), data.size());

        ph.length = static_cast<std::uint32_t>(data.size());

        writer.reset(result);

        writer << ph;
        writer.write(data);

        result.shrink_to_fit();

        return result;
    }

    template <
        class _Ty,
        class _CheckSum = empty_checksum>
    std::vector<uint8_t> serialize(
        const void *buffer,
        size_t bufsize,
        const _Ty &value,
        _CheckSum checksum = empty_checksum{})
    {
        std::vector<uint8_t> result{};

        bytes_writer_bounded writer{(uint8_t *)buffer, bufsize};

        // serialization
        writer << value;

        auto length = writer.count();

        result.reserve(writer.count() + sizeof(packer_header));

        // insert packer header
        packer_header ph{};

        ph.set_version(VERSION);

        ph.crc.crc32 = checksum((uint8_t *)buffer, length);

        ph.length = static_cast<std::uint32_t>(length);

        bytes_writer data_writer{result};

        data_writer << ph;
        data_writer.write((uint8_t *)buffer, length);

        result.shrink_to_fit();

        return result;
    }

    template <
        class _Ty,
        class _CheckSum = empty_checksum,
        std::enable_if_t<std::is_default_constructible_v<_Ty>, int> = 0>
    _Ty deserialize(const std::vector<uint8_t> &data, _CheckSum checksum = empty_checksum{})
    {
        bytes_reader reader{data};

        packer_header ph{};

        reader >> ph;

        // check header
        if (ph.version != VERSION)
            return _Ty{};

        // check checksum
        std::uint32_t crc = checksum(data.data() + sizeof(packer_header), ph.length);
        if (crc != ph.crc.crc32)
            return _Ty{};

        // perform deserialize
        return deserialize_object<_Ty>(reader);
    }

    template <
        class _Ty,
        class _CheckSum = empty_checksum,
        std::enable_if_t<std::is_default_constructible_v<_Ty>, int> = 0>
    _Ty deserialize(
        const void *buffer,
        size_t length,
        _CheckSum checksum = empty_checksum{})
    {
        bytes_reader_bounded reader{(uint8_t *)buffer, length};

        packer_header ph{};

        reader >> ph;

        // check header
        if (ph.version != VERSION)
            return _Ty{};

        // check checksum
        std::uint32_t crc = checksum((uint8_t *)buffer + sizeof(packer_header), ph.length);
        if (crc != ph.crc.crc32)
            return _Ty{};

        // perform deserialize
        return deserialize_object<_Ty>(reader);
    }
}