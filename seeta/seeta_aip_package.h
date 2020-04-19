//
// Created by kier on 2020/4/18.
//

#ifndef _INC_SEETA_AIP_PACKAGE_H
#define _INC_SEETA_AIP_PACKAGE_H


#include "seeta_aip.h"
#include "seeta_aip_cpp.h"

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
                    while (models[length]) {
                        ++length;
                    }

                    std::vector<std::string> cpp_models(models, models + length);

                    try {
                        raw->init(*device, cpp_models);
                    } catch (const Exception &e) {
                        return e.errcode();
                    }

                    *paip = static_cast<SeetaAIPHandle>(wrapper.release());
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

        template<typename T, typename = typename std::enable_if<std::is_base_of<Package, T>::value>::type>
        inline SeetaAIP describe_aip() {
            using Wrapper = PackageWrapper<T>;

            SeetaAIP aip;
            aip.error = Wrapper::Error;
            aip.free = Wrapper::Free;
            aip.reset = Wrapper::Reset;
            aip.forward = Wrapper::Forward;
            aip.property = Wrapper::Property;
            aip.set = Wrapper::Set;
            aip.get = Wrapper::Get;

            return aip;
        }
    }
}

#endif //_INC_SEETA_AIP_PACKAGE_H
