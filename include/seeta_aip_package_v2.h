//
// Created by SeetaTech on 2020/4/30.
//

#ifndef _INC_SEETA_AIP_PACKAGE_V2_H
#define _INC_SEETA_AIP_PACKAGE_V2_H

#include "seeta_aip_package.h"
#include <map>
#include <climits>

namespace seeta {
    namespace aip {
        template<typename T>
        static inline T min(T a, T b, T c) {
            return std::min<T>(std::min<T>(a, b), c);
        }

        inline int edit_distance(const std::string &lhs, const std::string &rhs) {
            const size_t M = lhs.length();  // rows
            const size_t N = rhs.length();  // cols

            if (M == 0) return int(N);
            if (N == 0) return int(M);

            std::unique_ptr<int[]> dist(new int[M * N]);
#define __EDIT_DIST(m, n) (dist[(m) * N + (n)])
            __EDIT_DIST(0, 0) = lhs[0] == rhs[0] ? 0 : 2;
            for (size_t n = 1; n < N; ++n) {
                __EDIT_DIST(0, n) = __EDIT_DIST(0, n - 1) + 1;
            }
            for (size_t m = 1; m < M; ++m) {
                __EDIT_DIST(m, 0) = __EDIT_DIST(m - 1, 0) + 1;
            }
            for (size_t m = 1; m < M; ++m) {
                for (size_t n = 1; n < N; ++n) {
                    if (lhs[m] == rhs[n]) {
                        __EDIT_DIST(m, n) = min(
                                __EDIT_DIST(m - 1, n),
                                __EDIT_DIST(m, n - 1),
                                __EDIT_DIST(m - 1, n - 1));
                    } else {
                        __EDIT_DIST(m, n) = min(
                                __EDIT_DIST(m - 1, n) + 1,
                                __EDIT_DIST(m, n - 1) + 1,
                                __EDIT_DIST(m - 1, n - 1) + 2);
                    }
                }
            }
            return dist[M * N - 1];
#undef __EDIT_DIST
        }

        class PackageV2 : public Package {
        public:
            using self = PackageV2;

            PackageV2() = default;

            ~PackageV2() override = default;

            enum PropertyType {
                Writeable = 0,
                ReadOnly = 1,
            };

            PackageV2(const PackageV2 &) = delete;

            PackageV2 &operator=(const PackageV2 &) = delete;

            using PropertySetter = std::function<void(double)>;
            using PropertyGetter = std::function<double()>;
            using ObjectSetter = std::function<void(const SeetaAIPObject &)>;
            using ObjectGetter = std::function<SeetaAIPObject()>;

            template<typename K, typename V>
            static void insert_map(std::map<K, V> &map, const K &k, const V &v) {
                auto it = map.find(k);
                if (it == map.end()) {
                    map.insert(std::make_pair(k, v));
                } else {
                    it->second = v;
                }
            }

            void bind_error(int32_t errcode, const std::string &message) {
                insert_map(m_error_message, errcode, message);
            }

            void bind_property(const std::string &name, const PropertyGetter &getter,
                               const PropertySetter &setter = nullptr) {
                insert_map(m_property_getter_map, name, getter);
                if (setter != nullptr) insert_map(m_property_setter_map, name, setter);
                m_property_buf.emplace_back(name);
            }

            void
            bind_property(const std::string &name, const ObjectGetter &getter, const ObjectSetter &setter = nullptr) {
                insert_map(m_object_getter_map, name, getter);
                if (setter != nullptr) insert_map(m_object_setter_map, name, setter);
                m_property_buf.emplace_back(name);
            }

            void bind_tag(uint32_t method_id, uint32_t label_index, int32_t label_value, const std::string &text) {
                insert_map(m_tag_message, std::make_tuple(method_id, label_index, label_value), text);

            }

            void bind_tag(uint32_t method_id, uint32_t label_index,
                          const std::vector<std::tuple<int32_t, std::string>> &text) {
                for (auto &pack : text) {
                    bind_tag(method_id, label_index, std::get<0>(pack), std::get<1>(pack));
                }
            }

            void bind_tag(uint32_t method_id, const std::vector<std::tuple<uint32_t, int32_t, std::string>> &text) {
                for (auto &pack : text) {
                    bind_tag(method_id, std::get<0>(pack), std::get<1>(pack), std::get<2>(pack));
                }
            }

            template<typename T, typename = typename std::enable_if<std::is_arithmetic<T>::value>::type>
            void bind_property(const std::string &name, T &value, PropertyType type = Writeable) {
                PropertyGetter getter = [&value]() { return static_cast<double>(value); };
                PropertySetter setter = nullptr;
                if (type == Writeable) setter = [&value](double arg) { value = static_cast<T>(arg); };
                bind_property(name, getter, setter);
            }

            void bind_property(const std::string &name, Object &value, PropertyType type = Writeable) {
                ObjectGetter getter = [&value]() { return *value.raw(); };
                ObjectSetter setter = nullptr;
                if (type == Writeable) setter = [&value](const SeetaAIPObject &arg) { value.raw(arg); };
                bind_property(name, getter, setter);
            }

            const char *error(int32_t errcode) final {
                auto key = errcode;
                auto it = m_error_message.find(key);
                if (it == m_error_message.end()) return nullptr;
                return it->second.c_str();
            }

            const char **property() final {
                if (m_property_tmp.size() != m_property_buf.size() + 1) {
                    m_property_tmp.clear();
                    for (auto &name : m_property_buf) {
                        m_property_tmp.push_back(name.c_str());
                    }
                    m_property_tmp.push_back(nullptr);
                }
                return m_property_tmp.data();
            }

            void setd(const std::string &name, double value) final {
                auto it = m_property_setter_map.find(name);
                if (it != m_property_setter_map.end()) {
                    it->second(value);
                    return;
                }
                if (setd2o(name, value)) return;
                if (has_getter(name)) {
                    std::ostringstream oss;
                    oss << "Property \"" << name << "\" is read-only, can not be written.";
                    throw Exception(-1, oss.str());
                }
                {
                    std::ostringstream oss;
                    oss << "Property \"" << name << "\" does not exists, "
                        << "did you mean \"" << fuzzy_property_name(name) << "\".";
                    throw Exception(-1, oss.str());
                }
            }

            double getd(const std::string &name) final {
                auto it = m_property_getter_map.find(name);
                if (it != m_property_getter_map.end()) {
                    return it->second();
                }
                double value;
                if (geto2d(name, value)) return value;
                {
                    std::ostringstream oss;
                    oss << "Property \"" << name << "\" does not exists, "
                        << "did you mean \"" << fuzzy_property_name(name) << "\".";
                    throw Exception(-1, oss.str());
                }
            }

            void set(const std::string &name, const SeetaAIPObject &object) final {
                auto it = m_object_setter_map.find(name);
                if (it != m_object_setter_map.end()) {
                    it->second(object);
                    return;
                }
                if (seto2d(name, object)) return;
                if (has_getter(name)) {
                    std::ostringstream oss;
                    oss << "Property \"" << name << "\" is read-only, can not be written.";
                    throw Exception(-1, oss.str());
                }
                {
                    std::ostringstream oss;
                    oss << "Property \"" << name << "\" does not exists, "
                        << "did you mean \"" << fuzzy_property_name(name) << "\".";
                    throw Exception(-1, oss.str());
                }
            }

            SeetaAIPObject get(const std::string &name) final {
                auto it = m_object_getter_map.find(name);
                if (it != m_object_getter_map.end()) {
                    return it->second();
                }
                SeetaAIPObject object = {};
                if (getd2o(name, object)) return object;
                {
                    std::ostringstream oss;
                    oss << "Property \"" << name << "\" does not exists, "
                        << "did you mean \"" << fuzzy_property_name(name) << "\".";
                    throw Exception(-1, oss.str());
                }
            }

            const char *tag(uint32_t method_id, uint32_t label_index, int32_t label_value) final {
                auto key = std::make_tuple(method_id, label_index, label_value);
                auto it = m_tag_message.find(key);
                if (it == m_tag_message.end()) return nullptr;
                return it->second.c_str();
            }

        private:
            bool has_getter(const std::string &name) {
                {
                    auto it = m_property_getter_map.find(name);
                    if (it != m_property_getter_map.end()) return true;
                }
                {
                    auto it = m_object_getter_map.find(name);
                    if (it != m_object_getter_map.end()) return true;
                }
                return false;
            }

            bool setd2o(const std::string &name, double value) {
                auto it = m_object_setter_map.find(name);
                if (it == m_object_setter_map.end()) return false;
                Tensor extra(SEETA_AIP_VALUE_FLOAT64, {});
                extra.data<double>()[0] = value;
                Object object(extra);
                it->second(object);
                return true;
            }

            static std::string get_extra_tensor(const SeetaAIPObject &object) {
                std::ostringstream oss;
                if (object.extra.data == nullptr) {
                    return "null";
                }
                switch (object.extra.type) {
                    default:
                        oss << "unknown";
                        break;
                    case SEETA_AIP_VALUE_BYTE:
                        oss << "byte";
                        break;
                    case SEETA_AIP_VALUE_FLOAT32:
                        oss << "float32";
                        break;
                    case SEETA_AIP_VALUE_INT32:
                        oss << "int32";
                        break;
                    case SEETA_AIP_VALUE_FLOAT64:
                        oss << "float64";
                        break;
                }
                oss << "[";
                for (uint32_t i = 0; i < object.extra.dims.size; ++i) {
                    if (i) oss << ", ";
                    oss << object.extra.dims.data[i];
                }
                oss << "]";
                return oss.str();
            }

            static double getd_for_object(const SeetaAIPObject &object) {
                switch (object.extra.type) {
                    default:
                        return 0;
                    case SEETA_AIP_VALUE_BYTE:
                        return reinterpret_cast<uint8_t *>(object.extra.data)[0];
                    case SEETA_AIP_VALUE_FLOAT32:
                        return reinterpret_cast<float *>(object.extra.data)[0];
                    case SEETA_AIP_VALUE_INT32:
                        return reinterpret_cast<int32_t *>(object.extra.data)[0];
                    case SEETA_AIP_VALUE_FLOAT64:
                        return reinterpret_cast<double *>(object.extra.data)[0];
                }
            }

            bool seto2d(const std::string &name, const SeetaAIPObject &object) {
                auto it = m_property_setter_map.find(name);
                if (it == m_property_setter_map.end()) return false;
                if (object.extra.data == nullptr || object.extra.dims.size != 0) {
                    std::ostringstream oss;
                    oss << "Property \"" << name << "\": Can not convert " << get_extra_tensor(object) << " to double.";
                    throw Exception(-1, oss.str());
                }
                it->second(getd_for_object(object));
                return true;
            }

            bool getd2o(const std::string &name, SeetaAIPObject &object) {
                auto it = m_property_getter_map.find(name);
                if (it == m_property_getter_map.end()) return false;
                auto value = it->second();
                Tensor extra(SEETA_AIP_VALUE_FLOAT64, {});
                extra.data<double>()[0] = value;
                m_object_value_tmp.rextra() = extra;
                object = m_object_value_tmp;
                return true;
            }

            bool geto2d(const std::string &name, double &value) {
                auto it = m_object_getter_map.find(name);
                if (it == m_object_getter_map.end()) return false;
                auto object = it->second();
                if (object.extra.data == nullptr || object.extra.dims.size != 0) {
                    std::ostringstream oss;
                    oss << "Property \"" << name << "\": Can not convert " << get_extra_tensor(object) << " to double.";
                    throw Exception(-1, oss.str());
                }
                value = getd_for_object(object);
                return true;
            }

            std::string fuzzy_property_name(const std::string &name) {
                if (m_property_buf.empty()) return "";
                int min_edit_distance = INT_MAX;
                std::string closest_name;
                for (auto &target_name : m_property_buf) {
                    int dist = edit_distance(name, target_name);
                    if (dist < min_edit_distance) {
                        closest_name = target_name;
                        min_edit_distance = dist;
                    }
                }
                return closest_name;
            }

        private:
            std::map<std::string, PropertySetter> m_property_setter_map;
            std::map<std::string, PropertyGetter> m_property_getter_map;
            std::map<std::string, ObjectSetter> m_object_setter_map;
            std::map<std::string, ObjectGetter> m_object_getter_map;

            std::vector<std::string> m_property_buf;
            std::vector<const char *> m_property_tmp = {nullptr};

            std::map<int32_t, std::string> m_error_message;
            std::map<std::tuple<uint32_t, uint32_t, int32_t>, std::string> m_tag_message;

            Object m_object_value_tmp;
        };
    }
}

#endif //_INC_SEETA_AIP_PACKAGE_V2_H
