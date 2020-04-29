//
// Created by SeetaTech on 2020/4/22.
//

#ifndef _INC_SEETA_AIP_ENGINE_H
#define _INC_SEETA_AIP_ENGINE_H

#include <memory>
#include "seeta_aip.h"
#include "seeta_aip_dll.h"
#include "seeta_aip_struct.h"

namespace seeta {
    namespace aip {
        class Library {
        public:
            using self = Library;

            explicit Library(const std::string &libname) {
                m_lib.reset(dlopen_v2(libname), dlclose);
                if (m_lib == nullptr) {
                    throw Exception(dlerror());
                }
            }

            Library() = default;

            ~Library() = default;

            template<typename FUNC>
            typename std::enable_if<std::is_function<FUNC>::value, FUNC *>::type
            symbol(const std::string &name) {
                auto func = reinterpret_cast<FUNC *>(dlsym(m_lib.get(), name.c_str()));
                if (func == nullptr) {
                    throw Exception(dlerror());
                }
                return func;
            }

            template<typename RET, typename... ARGS>
            using FUNCTION = RET(ARGS...);

            template<typename RET, typename... ARGS>
            FUNCTION<RET, ARGS...> *function(const std::string &name) {
                return symbol<FUNCTION<RET, ARGS...>>(name);
            }

        private:
            std::shared_ptr<void> m_lib;
        };

        class Engine {
        public:
            using self = Engine;

            enum Type {
                STATIC = 0,
                DYNAMIC = 1,
            };
            Engine(const Engine &) = delete;

            Engine &operator=(const Engine &) = delete;

            explicit Engine(seeta_aip_load_entry *entry) {
                m_type = STATIC;
                auto errcode = entry(&m_aip, sizeof(SeetaAIP));
                if (errcode != 0) {
                    switch (errcode) {
                        default:
                            throw Exception("Load failed.");
                        case SEETA_AIP_LOAD_SIZE_NOT_ENOUGH:
                            throw Exception("Size not enough.");
                        case SEETA_AIP_LOAD_UNHANDLED_INTERNAL_ERROR:
                            throw Exception("Unhandled internal error.");
                    }
                }
                if (m_aip.aip_version != SEETA_AIP_VERSION) {
                    throw Exception(SEETA_AIP_LOAD_AIP_VERSION_MISMATCH, "AIP version mismatch.");
                }
            }

            explicit Engine(const std::string &libname) {
                m_lib = Library(libname);
                m_type = DYNAMIC;
                auto entry = m_lib.symbol<seeta_aip_load_entry>(entry_name);
                auto errcode = entry(&m_aip, sizeof(SeetaAIP));
                if (errcode != 0) {
                    switch (errcode) {
                        default:
                            throw Exception("Load failed.");
                        case SEETA_AIP_LOAD_SIZE_NOT_ENOUGH:
                            throw Exception("Size not enough.");
                        case SEETA_AIP_LOAD_UNHANDLED_INTERNAL_ERROR:
                            throw Exception("Unhandled internal error.");
                    }
                }
                if (m_aip.aip_version != SEETA_AIP_VERSION) {
                    throw Exception(SEETA_AIP_LOAD_AIP_VERSION_MISMATCH, "AIP version mismatch.");
                }
            }

            Type type() const { return m_type; }

            const SeetaAIP &aip() const { return m_aip; }

        private:
            const char *entry_name = "seeta_aip_load";
            Library m_lib;
            Type m_type;
            SeetaAIP m_aip = {};
        };

        class Instance {
        public:
            using self = Instance;

            struct Result {
                struct {
                    SeetaAIPImageData *data = nullptr;
                    uint32_t size = 0;
                } images;

                struct {
                    SeetaAIPObject *data = nullptr;
                    uint32_t size = 0;
                } objects;
            };

            Instance(const Instance &) = delete;

            Instance &operator=(const Instance &) = delete;

        private:
            void construct(const SeetaAIP &aip, const Device &device, const std::vector<std::string> &models) {
                this->construct(aip, device, models, {});
            }

            void construct(const SeetaAIP &aip, const Device &device,
                    const std::vector<std::string> &models,
                    const std::vector<Object> &args) {
                m_handle = nullptr;
                m_aip = aip;

                auto c_args = Convert(args);

                std::vector<const char *> c_models;
                c_models.reserve(models.size());
                for (auto &model : models) { c_models.emplace_back(model.c_str()); }
                c_models.emplace_back(nullptr);
                auto errcode = m_aip.create(&m_handle, device, c_models.data(), c_args.data(), uint32_t(c_args.size()));
                if (errcode) throw Exception(errcode, m_aip.error(nullptr, errcode));
            }

        public:

            Instance(const SeetaAIP &aip, const Device &device, const std::vector<std::string> &models, const std::vector<Object> &args) {
                this->construct(aip, device, models, args);
            }

            Instance(const std::shared_ptr<Engine> &engine, const Device &device, const std::vector<std::string> &models, const std::vector<Object> &args) {
                auto &aip = engine->aip();
                if (engine->type() == Engine::DYNAMIC) {
                    m_engine = engine;
                }

                this->construct(aip, device, models, args);
            }

            Instance(const std::string &libname, const Device &device, const std::vector<std::string> &models, const std::vector<Object> &args) {
                auto engine = std::make_shared<Engine>(libname);

                auto &aip = engine->aip();
                if (engine->type() == Engine::DYNAMIC) {
                    m_engine = engine;
                }

                this->construct(aip, device, models, args);
            }

            Instance(seeta_aip_load_entry *entry, const Device &device, const std::vector<std::string> &models, const std::vector<Object> &args) {
                auto engine = std::make_shared<Engine>(entry);

                auto &aip = engine->aip();
                if (engine->type() == Engine::DYNAMIC) {
                    m_engine = engine;
                }

                this->construct(aip, device, models, args);
            }

            Instance(const SeetaAIP &aip, const Device &device, const std::vector<std::string> &models)
                : self(aip, device, models, {}) {}


            Instance(const std::shared_ptr<Engine> &engine, const Device &device, const std::vector<std::string> &models)
                    : self(engine, device, models, {}) {}


            Instance(const std::string &libname, const Device &device, const std::vector<std::string> &models)
                    : self(libname, device, models, {}) {}


            Instance(seeta_aip_load_entry *entry, const Device &device, const std::vector<std::string> &models)
                    : self(entry, device, models, {}) {}

            ~Instance() {
                if (m_handle) {
                    m_aip.free(m_handle);
                }
            }

            SeetaAIPHandle handle() const { return m_handle; }

            void dispose() {
                if (m_handle) {
                    m_aip.free(m_handle);
                    m_handle = nullptr;
                }
            }

            const char *error(int32_t errcode) {
                auto message = m_aip.error(m_handle, errcode);
                if (message == nullptr) return "";
                return message;
            }

            void reset() {
                auto errcode = m_aip.reset(m_handle);
                if (errcode) throw Exception(errcode, m_aip.error(nullptr, errcode));
            }

            std::vector<std::string> property() {
                int32_t *values = nullptr;
                auto property_names = m_aip.property(m_handle);
                if (property_names == nullptr) return {};
                std::vector<std::string> result;
                while (*property_names) {
                    result.emplace_back(*property_names);
                    ++property_names;
                }
                return result;
            }

            void setd(const std::string &name, double value) {
                auto errcode = m_aip.setd(m_handle, name.c_str(), value);
                if (errcode) throw Exception(errcode, m_aip.error(nullptr, errcode));
            }


            double getd(const std::string &name) {
                double value = 0;
                auto errcode = m_aip.getd(m_handle, name.c_str(), &value);
                if (errcode) throw Exception(errcode, m_aip.error(nullptr, errcode));
                return value;
            }

            void set(const std::string &name, const SeetaAIPObject &value) {
                auto errcode = m_aip.set(m_handle, name.c_str(), &value);
                if (errcode) throw Exception(errcode, m_aip.error(nullptr, errcode));
            }

            void set(const std::string &name, const Object &value) {
                return set(name, *value.raw());
            }

            SeetaAIPObject get(const std::string &name) {
                SeetaAIPObject value = {};
                auto errcode = m_aip.get(m_handle, name.c_str(), &value);
                if (errcode) throw Exception(errcode, m_aip.error(nullptr, errcode));
                return value;
            }

            Result forward(uint32_t method_id,
                           const struct SeetaAIPImageData *images, uint32_t images_size,
                           const struct SeetaAIPObject *objects, uint32_t objects_size) {
                Result result;
                auto errcode = m_aip.forward(m_handle,
                                             method_id,
                                             images, images_size, objects, objects_size,
                                             &result.objects.data, &result.objects.size,
                                             &result.images.data, &result.images.size);
                if (errcode) throw Exception(errcode, m_aip.error(nullptr, errcode));
                return result;
            }

            Result forward(uint32_t method_id,
                           const std::vector<SeetaAIPImageData> &images,
                           const std::vector<SeetaAIPObject> &objects) {
                return forward(method_id,
                               images.data(), uint32_t(images.size()),
                               objects.data(), uint32_t(objects.size()));
            }

            static std::vector<SeetaAIPImageData> Convert(const std::vector<ImageData> &array) {
                std::vector<SeetaAIPImageData> cvt;
                cvt.reserve(array.size());
                for (auto &obj : array) {
                    cvt.emplace_back(*obj.raw());
                }
                return cvt;
            }

            static std::vector<SeetaAIPObject> Convert(const std::vector<Object> &array) {
                std::vector<SeetaAIPObject> cvt;
                cvt.reserve(array.size());
                for (auto &obj : array) {
                    cvt.emplace_back(*obj.raw());
                }
                return cvt;
            }

            Result forward(uint32_t method_id,
                           const std::vector<ImageData> &images,
                           const std::vector<Object> &objects) {
                return forward(method_id, Convert(images), Convert(objects));
            }

            Result forward(uint32_t method_id,
                           const std::vector<SeetaAIPImageData> &images,
                           const std::vector<Object> &objects) {
                return forward(method_id, images, Convert(objects));
            }

            Result forward(uint32_t method_id,
                           const std::vector<ImageData> &images,
                           const std::vector<SeetaAIPObject> &objects) {
                return forward(method_id, Convert(images), objects);
            }

            Result forward(uint32_t method_id,
                           const std::vector<SeetaAIPImageData> &images) {
                return forward(method_id, images, std::vector<SeetaAIPObject>());
            }

            Result forward(uint32_t method_id,
                           const std::vector<ImageData> &images) {
                return forward(method_id, Convert(images), std::vector<SeetaAIPObject>());
            }

            Result forward(uint32_t method_id, const SeetaAIPImageData &image, const std::vector<SeetaAIPObject> &objects) {
                return forward(method_id, std::vector<SeetaAIPImageData>({image}), objects);
            }

            Result forward(uint32_t method_id, const SeetaAIPImageData &image) {
                return forward(method_id, image, std::vector<SeetaAIPObject>());
            }

            Result forward(uint32_t method_id, const SeetaAIPImageData &image, const std::vector<Object> &objects) {
                return forward(method_id, image, Convert(objects));
            }

            const char *c_tag(uint32_t method_id, uint32_t label_index, int32_t label_value) {
                return m_aip.tag(m_handle, method_id, label_index, label_value);
            }

            std::string tag(uint32_t method_id, uint32_t label_index, int32_t label_value) {
                auto c_str = c_tag(method_id, label_index, label_value);
                if (c_str) return c_str;
                return "";
            }

        private:
            SeetaAIP m_aip = {};
            SeetaAIPHandle m_handle = nullptr;
            std::shared_ptr<Engine> m_engine;
        };
    }
}

#endif //_INC_SEETA_AIP_ENGINE_H
