#include <list>
#include <vector>
#include <string>
#include <unordered_map>
#include <forward_list>
#include <new>
#include <sstream>
#include <queue>
#include <fstream>

#include "zpacker.hpp"

struct Row
{
    uint16_t value;
    std::vector<int> data;

    std::size_t get_size() const
    {
        // can also be: sizeof(value) + zeus::get_size(data)
        return zpacker::get_size(value) + zpacker::get_size(data);
    }

    template <class _Writer>
    void serialize(_Writer &writer) const
    {
        writer << value << data;
    }

    template <class _Reader>
    static Row deserialize(_Reader &reader)
    {
        Row self{};

        reader >> self.value >> self.data;

        return self;
    }

    std::string print() const
    {
        std::stringstream ss;

        ss << "{" << "value: " << value << ",";

        ss << "data: [";

        auto it = data.begin();
        while (it != data.end())
        {
            ss << *it;
            if (++it != data.end())
                ss << ", ";
        }

        ss << "]}";

        return ss.str();
    }
};

struct Complicated
{
    Complicated()
        : name(L"jacky")
    {
        map.emplace(1, Row{1, {1, 1, 1}});
        map.emplace(2, Row{2, {2, 2, 2}});
        map.emplace(3, Row{3, {3, 3, 3}});
        map.emplace(4, Row{4, {4, 4, 4}});
        map.emplace(5, Row{5, {5, 5, 5}});
    }

    std::wstring name;
    std::unordered_map<uint32_t, Row> map;

    std::size_t get_size() const
    {
        return zpacker::get_size(name) + zpacker::get_size(map);
    }

    template <class _Writer>
    void serialize(_Writer &writer) const
    {
        writer << name << map;
    }

    template <class _Reader>
    static Complicated deserialize(_Reader &reader)
    {
        Complicated self{};

        reader >> self.name >> self.map;

        return self;
    }
};

struct Streamable
{
    std::vector<std::string> data { "1", "2", "3", "4" };

    template <class _Writer>
    void serialize(_Writer& writer) const
    {
        writer << data;
    }

    template <class _Reader>
    static Streamable deserialize(_Reader& reader)
    {
        Streamable s{};

        reader >> s.data;
    }

    friend std::ostream& operator << (std::ostream& s, const Streamable& o)
    {
        std::vector<uint8_t> serialized{};

        zpacker::bytes_writer writer{serialized};

        zpacker::serialize_object(writer, o);

        return s.write(reinterpret_cast<const char*>(serialized.data()), serialized.size());
    }
};

void association_container_example()
{
    std::unordered_map<std::string, uint32_t> map1{{"Jacky", 68}, {"Element", 97}, {"Bob", 45}};

    auto data1 = zpacker::serialize(map1);

    auto object = zpacker::deserialize<decltype(map1)>(data1);

    std::for_each(object.begin(), object.end(), [](const auto &v)
                  { printf("name: %s, score: %d\n", v.first.c_str(), v.second); });
}

void sequence_container_example()
{
    std::list<int> bin = {1, 2, 3, 4};

    std::vector<uint8_t> buffer;

    zpacker::bytes_writer writer{buffer};

    writer << bin;

    zpacker::bytes_reader reader{buffer};

    auto data = reader.read<std::vector<int>>();

    std::for_each(data.begin(), data.end(), [](const auto &v)
                  { printf("value = %d\n", v); });
}

void composite_example()
{
    std::vector<uint8_t> buffer;
    zpacker::bytes_writer writer{buffer};

    Complicated complicated{};

    writer << complicated;

    zpacker::bytes_reader reader{buffer};

    reader.reset(&buffer);

    auto data = reader.read<decltype(complicated)>();

    printf("name = %ls\n", data.name.c_str());
    printf("dictionary:\n");

    std::for_each(data.map.begin(), data.map.end(), [](const auto &v)
                  { printf("[%d, %s]\n", v.first, v.second.print().c_str()); });

    /* pack the serialized data of object */
    auto packed = zpacker::serialize(complicated);

    /* unpack the serialized data */
    auto object = zpacker::deserialize<Complicated>(packed);
}

void array_example()
{
    std::array<int, 5> arr1 = {1, 2, 3, 4, 5};

    auto bin1 = zpacker::serialize(arr1);

    // this is an error, std::array can not be dynamically construct
    // auto object = zeus::deserialize<decltype(arr1)>(bin1);

    /* deserialize into a std::vector<int> */
    auto object = zpacker::deserialize<std::vector<int>>(bin1);

    std::for_each(object.begin(), object.end(), [](const auto &v)
                  { printf("value = %d\n", v); });
}

void forward_list_example()
{
    auto bin1 = zpacker::serialize(std::forward_list<int>{1, 2, 3, 4});

    /* deserialize into a std::queue */
    auto object = zpacker::deserialize<std::deque<int>>(bin1);

    while (!object.empty())
    {
        printf("value = %d\n", object.front());

        object.pop_front();
    }
}

void variant_example()
{
    std::vector<uint8_t> buffer;
    std::variant<int, char, std::wstring> v1{L"serialization"};

    zpacker::bytes_writer writer{buffer};

    zpacker::serialize_object(writer, v1);

    zpacker::bytes_reader reader{buffer};

    auto object = zpacker::deserialize_object<decltype(v1)>(reader);

    printf("index = %zd, value = %ls\n", object.index(), std::get<std::wstring>(object).c_str());
}

void tuple_example()
{
    std::tuple<std::string, uint32_t, std::string, uint32_t> t1{"192.168.10.1", 3768, "202.113.76.68", 80};

    auto data1 = zpacker::serialize(t1);

    auto object = zpacker::deserialize<decltype(t1)>(data1);

    printf("%s:%u -> %s:%u\n", std::get<0>(object).c_str(), std::get<1>(object), std::get<2>(object).c_str(), std::get<3>(object));
}

void get_size_example()
{
    std::variant<std::wstring, int, long double> var1{L"Bob"};

    auto size1 = zpacker::get_size<decltype(var1)>(var1);

    std::tuple<std::string, int, double> var2{"Bob", 3435, 3.1415926};

    auto size2 = zpacker::get_size<decltype(var2)>(var2);

    std::variant<std::list<int>, long, float, char> var3{(long)4};

    auto size3 = zpacker::get_size<decltype(var3)>(var3);

    std::tuple<int, std::wstring, std::vector<std::string>, float> var4{8, L"Bob", {"Jacky", "Element", "ElementX"}, 3.14f};

    auto size4 = zpacker::get_size<decltype(var4)>(var4);

    printf("size1 = %zd, size2 = %zd, size3 = %zd, size4 = %zd\n", size1, size2, size3, size4);
}

void test_multi_map()
{
    std::unordered_multimap<std::string, int> multimap1{{"Jacky", 64}, {"Jacky", 32}};

    auto data1 = zpacker::serialize(multimap1);

    auto object = zpacker::deserialize<decltype(multimap1)>(data1);

    std::for_each(object.begin(), object.end(), [](const decltype(multimap1)::value_type &v)
                  { printf("name: %s, salary: %d\n", v.first.c_str(), v.second); });
}

struct CustomType
{
    uint32_t id{};
    std::string name{};
    uint32_t salary{};
    std::list<std::string> friends{};

    CustomType() : id(0), name("jacky"), salary(3267), friends{"Bob", "Element"} {}

    // user defined `serialize` method for static binding
    template <class _Writer>
    void serialize(_Writer &writer) const
    {
        writer << id << name << friends;
    }

    // user defined `deserialize` method for static binding
    template <class _Reader>
    static CustomType deserialize(_Reader &reader)
    {
        CustomType self{};

        reader >> self.id >> self.name >> self.friends;

        return self;
    }
};

void custom_type_example()
{
    CustomType custom{};

    auto data = zpacker::serialize(custom);

    auto object = zpacker::deserialize<decltype(custom)>(data);
}

void stream_example()
{
    Streamable s{};
    
    std::ofstream os;

    os.open("1.bin", std::ios::binary);

    os << s;

    os.flush();
    os.close();
}

int main(int argc, char const *argv[])
{
    array_example();
    forward_list_example();
    composite_example();

    variant_example();
    tuple_example();
    get_size_example();

    sequence_container_example();
    association_container_example();

    test_multi_map();

    stream_example();

    struct Test 
    {
        Test() = delete;

        Test(Test&&) = default;
    };

    static_assert(std::is_move_constructible_v<Test>);

    return 0;
}
