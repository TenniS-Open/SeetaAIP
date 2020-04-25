//
// Created by kier on 2020/4/18.
//

#ifndef _INC_SEETA_AIP_PACKAGE_H
#define _INC_SEETA_AIP_PACKAGE_H


#include "seeta_aip.h"
#include "seeta_aip_struct.h"

#include <vector>
#include <string>
#include <type_traits>

namespace seeta {
    namespace aip {
        template<typename T, typename Enable = void>
        class PackageWrapper;

        class Package {
        public:
            using self = Package;

            class Result {
            public:
                std::vector<ImageData> images;
                std::vector<Object> objects;
            };

            virtual ~Package() = default;

            virtual const char *error(int32_t errcode) = 0;

            virtual void init(const SeetaAIPDevice &device, const std::vector<std::string> &models) = 0;

            virtual std::vector<int32_t> property() = 0;

            virtual void set(int32_t property_id, double value) = 0;

            virtual double get(int32_t property_id) = 0;

            virtual void reset() {}

            virtual const char *tag(uint32_t method_id, uint32_t label_index, int32_t label_value) { return nullptr; }

            virtual void forward(
                    uint32_t method_id,
                    const std::vector<SeetaAIPImageData> &images,
                    const std::vector<SeetaAIPObject> &objects) = 0;

            const Result &const_result() const { return result; }

        protected:
            Result result;
        };

        template<typename T>
        class PackageWrapper<T, typename std::enable_if<std::is_base_of<Package, T>::value>::type> {
        public:
            using self = PackageWrapper;
            using Raw = T;

            static const char *Error(SeetaAIPHandle aip, int32_t errcode) {
                switch (errcode) {
                    default:
                        break;
                    case SEETA_AIP_NO_ERROR:
                        return "No error.";
                    case SEETA_AIP_ERROR_UNHANDLED_INTERNAL_ERROR:
                        return "Unhandled internal error.";
                    case SEETA_AIP_ERROR_BAD_ALLOC:
                        return "Catch std::back_alloc.";
                    case SEETA_AIP_ERROR_OUT_OF_MEMORY:
                        return "Out of memory.";
                    case SEETA_AIP_ERROR_EMPTY_PACKAGE_HANDLE:
                        return "Got emtpy package handle (==0).";
                    case SEETA_AIP_ERROR_PROPERTY_NOT_EXISTS:
                        return "Property not exists.";

                    case SEETA_AIP_ERROR_METHOD_ID_OUT_OF_RANGE:
                        return "Method ID out of range.";
                    case SEETA_AIP_ERROR_DEVICE_NOT_SUPPORTED:
                        return "Device not supported.";
                    case SEETA_AIP_ERROR_DEVICE_ID_OUT_OF_RANGE:
                        return "Device ID out of range.";
                    case SEETA_AIP_ERROR_MODEL_MISSING:
                        return "Model missing.";

                    case SEETA_AIP_ERROR_MISSING_REQUIRED_INPUT_IMAGE:
                        return "Missing required input image.";
                    case SEETA_AIP_ERROR_MISSING_REQUIRED_INPUT_OBJECT:
                        return "Missing required input object.";
                }
                try {
                    auto wrapper = static_cast<self *>((void *) aip);
                    if (wrapper == nullptr && errcode == -1) {
                        return "Unhandled internal error.";
                    }
                    if (errcode == -1) {
                        return wrapper->m_error_message.c_str();
                    }
                    if (wrapper == nullptr) {
                        T tmp;
                        Package *raw = &tmp;
                        return raw->error(errcode);
                    }
                    Package *raw = wrapper->m_raw.get();
                    return raw->error(errcode);
                } catch (const std::exception &) {
                    return "Unhandled internal error.";
                }
            }

            static int32_t Create(SeetaAIPHandle *paip,
                                  const SeetaAIPDevice *device,
                                  const char **models) {
                try {
                    *paip = nullptr;

                    std::unique_ptr<self> wrapper(new self);
                    wrapper->m_raw.reset(new T);
                    Package *raw = wrapper->m_raw.get();

                    auto length = 0;
                    while (models && models[length]) {
                        ++length;
                    }

                    std::vector<std::string> cpp_models(models, models + length);

                    try {
                        raw->init(*device, cpp_models);
                    } catch (const Exception &e) {
                        return e.errcode();
                    }

                    *paip = static_cast<SeetaAIPHandle>((void*)wrapper.release());
                    return 0;
                } catch (const std::bad_alloc &) {
                    return SEETA_AIP_ERROR_BAD_ALLOC;
                } catch (const std::exception &) {
                    return SEETA_AIP_ERROR_UNHANDLED_INTERNAL_ERROR;
                }
            }

            static int32_t Free(SeetaAIPHandle aip) {
                try {
                    auto wrapper = static_cast<self *>((void *) aip);
                    delete wrapper;
                    return 0;
                } catch (const std::exception &) {
                    return SEETA_AIP_ERROR_UNHANDLED_INTERNAL_ERROR;
                }
            }

            static int32_t Reset(SeetaAIPHandle aip) {
                try {
                    if (aip == nullptr) return SEETA_AIP_ERROR_EMPTY_PACKAGE_HANDLE;
                    auto wrapper = static_cast<self *>((void *) aip);
                    Package *raw = wrapper->m_raw.get();
                    try {
                        raw->reset();
                    } catch (const Exception &e) {
                        wrapper->m_error_message = e.message();
                        return e.errcode();
                    }
                    return 0;
                } catch (const std::exception &) {
                    return SEETA_AIP_ERROR_UNHANDLED_INTERNAL_ERROR;
                }
            }

            static int32_t Set(SeetaAIPHandle aip, int32_t property_id, double value) {
                try {
                    if (aip == nullptr) return SEETA_AIP_ERROR_EMPTY_PACKAGE_HANDLE;
                    auto wrapper = static_cast<self *>((void *) aip);
                    Package *raw = wrapper->m_raw.get();
                    try {
                        raw->set(property_id, value);
                    } catch (const Exception &e) {
                        wrapper->m_error_message = e.message();
                        return e.errcode();
                    }
                    return 0;
                } catch (const std::exception &) {
                    return SEETA_AIP_ERROR_UNHANDLED_INTERNAL_ERROR;
                }
            }

            static int32_t Get(SeetaAIPHandle aip, int32_t property_id, double *pvalue) {
                try {
                    if (aip == nullptr) return SEETA_AIP_ERROR_EMPTY_PACKAGE_HANDLE;
                    auto wrapper = static_cast<self *>((void *) aip);
                    Package *raw = wrapper->m_raw.get();
                    try {
                        *pvalue = raw->get(property_id);
                    } catch (const Exception &e) {
                        wrapper->m_error_message = e.message();
                        return e.errcode();
                    }
                    return 0;
                } catch (const std::exception &) {
                    return SEETA_AIP_ERROR_UNHANDLED_INTERNAL_ERROR;
                }
            }

            static int32_t Property(SeetaAIPHandle aip, int32_t **property) {
                try {
                    if (aip == nullptr) return SEETA_AIP_ERROR_EMPTY_PACKAGE_HANDLE;
                    auto wrapper = static_cast<self *>((void *) aip);
                    Package *raw = wrapper->m_raw.get();
                    try {
                        wrapper->m_property = raw->property();
                        wrapper->m_property.push_back(0);
                        *property = wrapper->m_property.data();
                    } catch (const Exception &e) {
                        wrapper->m_error_message = e.message();
                        return e.errcode();
                    }
                    return 0;
                } catch (const std::exception &) {
                    return SEETA_AIP_ERROR_UNHANDLED_INTERNAL_ERROR;
                }
            }

            static int32_t Forward(SeetaAIPHandle aip,
                                   uint32_t method_id,
                                   const struct SeetaAIPImageData *images, uint32_t images_size,
                                   const struct SeetaAIPObject *objects, uint32_t objects_size,
                                   struct SeetaAIPObject **result_objects, uint32_t *result_objects_size,
                                   struct SeetaAIPImageData **result_images, uint32_t *result_images_size) {
                try {
                    if (aip == nullptr) return SEETA_AIP_ERROR_EMPTY_PACKAGE_HANDLE;
                    auto wrapper = static_cast<self *>((void *) aip);
                    Package *raw = wrapper->m_raw.get();
                    try {
                        *result_objects = nullptr;
                        *result_objects_size = 0;
                        *result_images = nullptr;
                        *result_images_size = 0;
                        wrapper->update_input(images, images_size, objects, objects_size);
                        auto &input_images = wrapper->m_input_images;
                        auto &input_objects = wrapper->m_input_objects;
                        raw->forward(method_id, input_images, input_objects);
                        wrapper->update_output(result_objects, result_objects_size, result_images, result_images_size);
                    } catch (const Exception &e) {
                        wrapper->m_error_message = e.message();
                        return e.errcode();
                    }
                    return 0;
                } catch (const std::exception &) {
                    return SEETA_AIP_ERROR_UNHANDLED_INTERNAL_ERROR;
                }
            }

            static const char *Tag(SeetaAIPHandle aip, uint32_t method_id, uint32_t label_index, int32_t label_value) {
                try {
                    if (aip == nullptr) return nullptr;
                    auto wrapper = static_cast<self *>((void *) aip);
                    Package *raw = wrapper->m_raw.get();
                    try {
                        return raw->tag(method_id, label_index, label_value);
                    } catch (const Exception &e) {
                        wrapper->m_error_message = e.message();
                        return nullptr;
                    }
                } catch (const std::exception &) {
                    return nullptr;
                }
            }

        private:
            std::shared_ptr<Raw> m_raw;
            std::string m_error_message;
            std::vector<int32_t> m_property;
            std::vector<SeetaAIPImageData> m_input_images;
            std::vector<SeetaAIPObject> m_input_objects;
            std::vector<SeetaAIPImageData> m_output_images;
            std::vector<SeetaAIPObject> m_output_objects;

        private:
            void update_input(
                    const struct SeetaAIPImageData *images, uint32_t images_size,
                    const struct SeetaAIPObject *objects, uint32_t objects_size) {
                m_input_images.clear();
                m_input_images.reserve(images_size);
                m_input_images.insert(m_input_images.end(), images, images + images_size);
                m_input_objects.clear();
                m_input_objects.reserve(objects_size);
                m_input_objects.insert(m_input_objects.end(), objects, objects + objects_size);
            }

            void update_output(
                    struct SeetaAIPObject **result_objects, uint32_t *result_objects_size,
                    struct SeetaAIPImageData **result_images, uint32_t *result_images_size) {
                Package *raw = m_raw.get();
                auto &result = raw->const_result();
                m_output_images.clear();
                m_output_images.reserve(result.images.size());
                for (auto &image : result.images) {
                    m_output_images.emplace_back(*image.raw());
                }
                m_output_objects.clear();
                m_output_objects.reserve(result.objects.size());
                for (auto &object : result.objects) {
                    m_output_objects.emplace_back(*object.raw());
                }
                *result_objects = m_output_objects.data();
                *result_objects_size = uint32_t(m_output_objects.size());
                *result_images = m_output_images.data();
                *result_images_size = uint32_t(m_output_images.size());
            }
        };

        class Header {
        public:
            const char *module = "";
            const char *description = "";
            const char *mID = "";
            const char *sID = "";
            const char *version = "0.0.0";
            const char **support = { nullptr };
        };

        inline void setup_aip_header(SeetaAIP &aip,
                const char *module,
                const char *description,
                const char *mID,
                const char *sID,
                const char *version,
                const char ** support) {
            aip.aip_version = SEETA_AIP_VERSION;

            aip.module = module;

            aip.description = description;
            aip.mID = mID;
            aip.sID = sID;
            aip.version = version;
            aip.support = support;
        }

        template<typename T>
        inline void setup_aip_header(SeetaAIP &aip) {
            aip.aip_version = SEETA_AIP_VERSION;

            T tmp;

            aip.module = tmp.module;

            aip.description = tmp.description;
            aip.mID = tmp.mID;
            aip.sID = tmp.sID;
            aip.version = tmp.version;
            aip.support = tmp.support;
        }

        template<typename T, typename = typename std::enable_if<std::is_base_of<Package, T>::value>::type>
        inline void setup_aip_entry(SeetaAIP &aip) {
            using Wrapper = PackageWrapper<T>;

            aip.error = Wrapper::Error;
            aip.create = Wrapper::Create;
            aip.free = Wrapper::Free;
            aip.reset = Wrapper::Reset;
            aip.forward = Wrapper::Forward;
            aip.property = Wrapper::Property;
            aip.set = Wrapper::Set;
            aip.get = Wrapper::Get;
            aip.tag = Wrapper::Tag;
        }

#define CHECK_AIP_SIZE(aip, size) \
            if (size < sizeof(SeetaAIP)) { \
                if (size > 4) aip->aip_version = SEETA_AIP_VERSION; \
                return SEETA_AIP_LOAD_SIZE_NOT_ENOUGH; \
            }
    }
}

#endif //_INC_SEETA_AIP_PACKAGE_H
