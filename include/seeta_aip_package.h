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

            virtual void create(const SeetaAIPDevice &device,
                    const std::vector<std::string> &models,
                    const std::vector<SeetaAIPObject> &args) = 0;

            virtual void free() = 0;

            virtual const char **property() = 0;

            virtual void setd(const std::string &name, double value) = 0;

            virtual double getd(const std::string &name) = 0;

            virtual void set(const std::string &name, const SeetaAIPObject &value) = 0;

            virtual SeetaAIPObject get(const std::string &name) = 0;

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
                    case SEETA_AIP_ERROR_MODEL_MISMATCH:
                        return "Model mismatch.";

                    case SEETA_AIP_ERROR_MISMATCH_REQUIRED_INPUT_IMAGE:
                        return "Mismatch required input image.";
                    case SEETA_AIP_ERROR_MISMATCH_REQUIRED_INPUT_OBJECT:
                        return "Mismatch required input object.";

                    case SEETA_AIP_ERROR_NULLPTR:
                        return "Got unexpected null pointer.";
                    case SEETA_AIP_ERROR_WRITE_READONLY_PROPERTY:
                        return "Write read-only property.";
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
                                  const char **models,
                                  const struct SeetaAIPObject *args, uint32_t argc) {
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

                    wrapper->update_input(nullptr, 0, args, argc);
                    auto &input_objects = wrapper->m_input_objects;

                    try {
                        raw->create(*device, cpp_models, input_objects);
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
                    if (aip == nullptr) return 0;
                    auto wrapper = static_cast<self *>((void *) aip);
                    Package *raw = wrapper->m_raw.get();
                    try {
                        raw->free();
                    } catch (const std::exception &) {
                        // no exception should be throw
                    }
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

            static int32_t SetD(SeetaAIPHandle aip, const char *name, double value) {
                try {
                    if (aip == nullptr) return SEETA_AIP_ERROR_EMPTY_PACKAGE_HANDLE;
                    if (name == nullptr) return SEETA_AIP_ERROR_NULLPTR;
                    auto wrapper = static_cast<self *>((void *) aip);
                    Package *raw = wrapper->m_raw.get();
                    try {
                        raw->setd(name, value);
                    } catch (const Exception &e) {
                        wrapper->m_error_message = e.message();
                        return e.errcode();
                    }
                    return 0;
                } catch (const std::exception &) {
                    return SEETA_AIP_ERROR_UNHANDLED_INTERNAL_ERROR;
                }
            }

            static int32_t GetD(SeetaAIPHandle aip, const char *name, double *pvalue) {
                try {
                    if (aip == nullptr) return SEETA_AIP_ERROR_EMPTY_PACKAGE_HANDLE;
                    if (name == nullptr) return SEETA_AIP_ERROR_NULLPTR;
                    auto wrapper = static_cast<self *>((void *) aip);
                    Package *raw = wrapper->m_raw.get();
                    try {
                        *pvalue = raw->getd(name);
                    } catch (const Exception &e) {
                        wrapper->m_error_message = e.message();
                        return e.errcode();
                    }
                    return 0;
                } catch (const std::exception &) {
                    return SEETA_AIP_ERROR_UNHANDLED_INTERNAL_ERROR;
                }
            }

            static const char **Property(SeetaAIPHandle aip) {
                try {
                    if (aip == nullptr) return nullptr;
                    auto wrapper = static_cast<self *>((void *) aip);
                    Package *raw = wrapper->m_raw.get();
                    try {
                        return raw->property();
                    } catch (const Exception &e) {
                        wrapper->m_error_message = e.message();
                        return nullptr;
                    }
                    return nullptr;
                } catch (const std::exception &) {
                    return nullptr;
                }
            }

            static int32_t Set(SeetaAIPHandle aip, const char *name, const SeetaAIPObject *pvalue) {
                try {
                    if (aip == nullptr) return SEETA_AIP_ERROR_EMPTY_PACKAGE_HANDLE;
                    if (name == nullptr) return SEETA_AIP_ERROR_NULLPTR;
                    if (pvalue == nullptr) return SEETA_AIP_ERROR_NULLPTR;
                    auto wrapper = static_cast<self *>((void *) aip);
                    Package *raw = wrapper->m_raw.get();
                    try {
                        raw->set(name, *pvalue);
                    } catch (const Exception &e) {
                        wrapper->m_error_message = e.message();
                        return e.errcode();
                    }
                    return 0;
                } catch (const std::exception &) {
                    return SEETA_AIP_ERROR_UNHANDLED_INTERNAL_ERROR;
                }
            }

            static int32_t Get(SeetaAIPHandle aip, const char *name, SeetaAIPObject *pvalue) {
                try {
                    if (aip == nullptr) return SEETA_AIP_ERROR_EMPTY_PACKAGE_HANDLE;
                    if (name == nullptr) return SEETA_AIP_ERROR_NULLPTR;
                    auto wrapper = static_cast<self *>((void *) aip);
                    Package *raw = wrapper->m_raw.get();
                    try {
                        *pvalue = raw->get(name);
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
            aip.setd = Wrapper::SetD;
            aip.getd = Wrapper::GetD;
            aip.tag = Wrapper::Tag;
            aip.set = Wrapper::Set;
            aip.get = Wrapper::Get;
        }

#define CHECK_AIP_SIZE(aip, size) \
            if (size < sizeof(SeetaAIP)) { \
                if (size > 4) aip->aip_version = SEETA_AIP_VERSION; \
                return SEETA_AIP_LOAD_SIZE_NOT_ENOUGH; \
            }
    }
}

#endif //_INC_SEETA_AIP_PACKAGE_H
