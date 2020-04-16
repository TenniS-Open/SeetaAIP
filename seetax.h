#ifndef _INC_SEETAX_H
#define _INC_SEETAX_H

/**
 * @author kaizhou.li
 * @date 2018-8-22
 * C99 support expected.
 * Error number: 0 for no error, else for error
 * The returns of `new pointer` must call `seetax_free_xxx` to free pointer.
 * The returns of `borrowed pointer` have no need to free outside.
 * Use associated `Container Document` to get help informations.
 * `Container Document` including but not limited to:
 *     1. type of APIs
 *     2. version of APIs
 *     3. meaningful description of label numbers, like {0: dog, 1: cat, ...}
 *     4. meaningful error message of error number, like {0: No error, 1: File not found, ...}
 *     5. Available properties those can be set in seetax_set, and default value
 *     6. Available properties those can be get in seetax_get
 *     7. [Optional] input image size
 *     8. [Optional] description of APIs
 */

#ifdef _MSC_VER
#ifdef SEETAX_EXPORTS
#define SEETAX_API __declspec(dllexport)
#else
#define SEETAX_API __declspec(dllimport)
#endif
#else
#define SEETAX_API __attribute__ ((visibility("default")))
#endif

#ifdef __cplusplus
extern "C" {
#endif

#include <stdint.h>

enum SEETAX_VALUE_TYPE
{
    SEETAX_VALUE_BYTE       = 0,    ///< byte type
    SEETAX_VALUE_FLOAT      = 1,    ///< float type with 4-bytes
};

enum SEETAX_SDK_TYPE
{
    SEETAX_UNKOWN_TYPE          = 0,                ///< unknown SDK API type
    SEETAX_OBJECT_DETECTION     = 1,                ///< object detection: input image then output object type and location
    SEETAX_CLASSIFICATION       = 2,                ///< classification: input image then output object type
    SEETAX_FEATURE_EXTRACTION   = 3,                ///< feature extraction: input image the output extracted features; input two features then output similarity
    SEETAX_IMAGE_PROCESSING     = 4,                ///< image processing: input image the output image. i.e. GAN, fully-connected-nets.
    SEETAX_PURE_DETECTION       = 5,                ///< pure detection: input image then output location without type
};

/**
 * \brief Point in image
 * Origin is the left upper corner.
 * X means coordinates from left to right.
 * Y means coordinates from top to bottom.
 */
struct SeetaXPoint
{
    float x;            ///< X-coordinate
    float y;            ///< Y-coordinate
};

/**
 * \brief Size of object
 */
struct SeetaXSize
{
    float width;        ///< width
    float height;       ///< height
};

enum SEETAX_SHAPE_TYPE
{
    /**
     * Unknown shape
     */
    SEETAX_UNKOWN_SHAPE     = 0,
    /**
     * with forced rotate=0, scale=1, size>=1
     * points represents points
     */
    SEETAX_POINTS           = 1,
    /**
     * with forced rotate=0, scale=1, size>=2
     * points represents multi lines:
     *     points[0]->points[1], points[1]->points[2], ..., points[size-2]->points[size-1]
     */
    SEETAX_LINES            = 2,
    /**
     * with forced scale=1, size=2
     * rotate represents the angle of rotation around the center point.
     * points[0] represents the left-top corner
     * points[1] represents the right-bottom corner
     */
    SEETAX_RECTANGLE        = 3,
    /**
     * with forced rotate=0, scale=1, size=3
     * rotate represents the angle of rotation around the center point.
     * points represents the first 3 points of parallelogram with anticlockwise
     */
    SEETAX_PARALLELOGRAM    = 4,
    /**
     * with forced rotate=0, scale=1, size>=2
     * points represents multi lines with anticlockwise:
     *     points[0]->points[1], points[1]->points[2], ...,
     *     points[size-2]->points[size-1], points[size-1]->points[0]
     */
    SEETAX_POLYGON          = 5,
    /**
     * with forced rotate=0, size=1
     * scale represents the radius
	 * points[0] represents the center
     */
    SEETAX_CIRCLE           = 6,
    /**
     * with forced rotate=0, scale=1, size=3
     * points[0] represents the left-top-front corner
     * points[1] represents the right-bottom-front corner
     * points[2] represents the right-top-back corner
     */
    SEETAX_CUBE             = 7,
};

/**
 * \brief shape of object
 */
struct SeetaXShape
{
    enum SEETAX_SHAPE_TYPE type;    ///< type of this shape
    struct SeetaXPoint *points;     ///< landmarks
    uint32_t size;                  ///< size of points
    float rotate;                   ///< using degree measure, positive value means anticlockwise
    float scale;                    ///< normally means scale of point's coordinate, specially means radius of a circle
};

/**
 * \brief detectable object
 */
struct SeetaXObject
{
    struct SeetaXShape shape;       ///< shape of object
    uint32_t label;                 ///< label index, reserved in SEETAX_PURE_DETECTION mode
    float score;                    ///< score of
};

enum SEETAX_DEVICE_TYPE
{
    SEETAX_DEVICE_AUTO  = 0,        ///< auto select best device, ** reserved field **
    SEETAX_DEVICE_CPU   = 1,        ///< force use CPU
    SEETAX_DEVICE_GPU   = 2,        ///< force use GPU
};

/**
 * \brief This is a dummy structure, to contain
 */
struct SeetaXContainer;

/**
 * \brief ImageData type
 */
struct SeetaXImageData
{
    enum SEETAX_VALUE_TYPE type;    ///< data value type, support SEETAX_VALUE_BYTE or SEETAX_VALUE_FLOAT
    void *data;                     ///< an array contains each pixel with dims [height, width, channels], the pixel type should be the given type
                                    // type=SEETAX_VALUE_BYTE represents decltype(*data)=uint8_t
                                    // type=SEETAX_VALUE_FLOAT represents decltype(*data)=float
    uint32_t height;                ///< height of image
    uint32_t width;                 ///< width of image
    uint32_t channels;              ///< channels of image
};

struct SeetaXObjectArray
{
    struct SeetaXObject *data;
    uint32_t size;
};

/**
 * \brief create seetax instance
 * \param [out] p_container return `new pointer` to instance
 * \param [in] path path to the data files, end with NULL, like {"data1.bin", "data2.txt", NULL}
 * \param [in] device running instance device
 * \param [in] id index of multi device
 * \return error number
 * \note remember to call `seetax_free_container` to free the `SeetaXContainer`
 * for device=SEETAX_DEVICE_GPU, id=1 means using GPU1
 */
SEETAX_API int32_t seetax_create_container(struct SeetaXContainer **const p_container, const char *const *path, enum SEETAX_DEVICE_TYPE device, uint32_t id);

/**
 * \brief free container
 * \param [in/out] p_container pointer to the return value of seetax_create_container
 * \note input NULL should be safe
 * \note *p_container should be NULL after free
 */
SEETAX_API void seetax_free_container(struct SeetaXContainer **const p_container);

/**
 * \brief set parameter
 * \param [in] container the return value of seetax_create_container
 * \param [in] prop_id property id, described with `Algorithm Container`
 * \param [in] value setting value
 */
SEETAX_API void seetax_set(struct SeetaXContainer *container, int32_t prop_id, double value);

/**
 * \brief get parameter
 * \param [in] container the return value of seetax_create_container
 * \param [in] prop_id property id, described with `Algorithm Container`
 * \return getting value
 */
SEETAX_API double seetax_get(struct SeetaXContainer *container, int32_t prop_id);

/**
 * \brief do object detection
 * \param [in] container the return value of seetax_create_container
 * \param [out] p_object_array return `new pointer` to array of `SeetaXObject`
 * \param [in] image inputing image
 * \return error number
 * \note use seetax_get_labels to get label string
 * \note remember to call `seeta_free_objects` to free the `SeetaXObjectArray *`
 */
SEETAX_API int32_t seetax_detect_objects(struct SeetaXContainer *container, struct SeetaXObjectArray **const p_object_array, const struct SeetaXImageData *image);

/**
 * \brief free objects, the return value of `seetax_detect_objects`
 * \param [in/out] p_objects pointer to objects ready to be free
 * \note input NULL should be safe
 * \note *p_objects should be NULL after free
 */
SEETAX_API void seetax_free_objects(struct SeetaXObjectArray **const p_objects);

/**
 * \brief do crop image, for classification or feature extraction
 * \param [in] container the return value of seetax_create_container
 * \param [out] p_patch return `new pointer` to `SeetaXImageData`
 * \param [in] image inputing image
 * \param [in] shape the shape guiding image cropper
 * \return error number
 * \note remember to call `seetax_free_image` to free the `SeetaXImageData *`
 * \note the output image always with type=SEETAX_VALUE_BYTE
 */
SEETAX_API int32_t seetax_crop_image(struct SeetaXContainer *container, struct SeetaXImageData **const p_patch, const struct SeetaXImageData *image, const struct SeetaXShape *shape);

/**
 * \brief free image, the return value of `seetax_crop_image` or `seetax_process_image`
 * \param [in/out] p_image pointer to image object ready to be free
 * \note input NULL should be safe
 * \note *p_image should be NULL after free
 */
SEETAX_API void seetax_free_image(struct SeetaXImageData **const p_image);

/**
 * \brief do classification
 * \param [in] container the return value of seetax_create_container
 * \param [out] label return the label of image patch
 * \param [in] image inputing image
 * \return error number
 * \note image can be original image or cropped image (often)
 * \note use seetax_get_labels to get label string
 */
SEETAX_API int32_t seetax_classify(struct SeetaXContainer *container, uint32_t *const label, const struct SeetaXImageData *image);


/**
 * \brief do classification
 * \param [in] container the return value of seetax_create_container
 * \param [out] p_feature_size return the size of features
 * \param [in] image inputing image
 * \return error number
 * \note image can be NULL
 */
SEETAX_API int32_t seetax_get_feature_size(struct SeetaXContainer *container, int32_t *const p_feature_size, const struct SeetaXImageData *image);


/**
 * \brief do feature extraction
 * \param [in] container the return value of seetax_create_container
 * \param [out] features return the features of image patch
 * \param [in] image inputing image
 * \return error number
 * \note image can be original image or cropped image (often)
 * \note the features should have enough space of seetax_get(SEETAX_PROP_FEATURE_SIZE)
 */
SEETAX_API int32_t seetax_extract_features(struct SeetaXContainer *container, float *const features, const struct SeetaXImageData *image);

/**
 * \brief do similarity calculation
 * \param [in] container the return value of seetax_create_container
 * \param [in] features1 the features return by `seetax_extract_features`
 * \param [in] features2 the features return by `seetax_extract_features`
 * \param [in] size feature size, use seetax_get(SEETAX_PROP_FEATURE_SIZE) if size <= 0
 * \return similarity
 */
SEETAX_API float seetax_calculate_similarity(struct SeetaXContainer *container, const float *features1, const float *features2, int32_t size);

/**
 * \brief do image processing
 * \param [in] container the return value of seetax_create_container
 * \param [out] p_processed_image return `new pointer` to `SeetaXImageData`
 * \param [in] image inputing image
 * \return error number
 * \note remember to call `seetax_free_image` to free the `SeetaXImageData *`
 * \note the output image always with type=SEETAX_VALUE_BYTE
 */
SEETAX_API int32_t seetax_process_image(struct SeetaXContainer *container, struct SeetaXImageData **const p_processed_image, const struct SeetaXImageData *image);

/**
 * \brief new image
 * \param [in] width width of new image
 * \param [in] height height of new image
 * \param [in] channels channels of new image
 * \param [in] type type of new image
 * \param [in] data data of new image, can be NULL for no initialization.
 * \return return `new pointer` to `SeetaXImageData`
 * \note the `data` must has space width*height*channels*sizeof(type) or be NULL
 * \note return NULL if failed
 * \note remember to call `seetax_free_image` to free the `SeetaXImageData *`
 */
SEETAX_API struct SeetaXImageData *seetax_new_image(uint32_t width, uint32_t height, uint32_t channels, enum SEETAX_VALUE_TYPE type, const void *data);

#ifdef __cplusplus
}
#endif

#endif    // _INC_SEETAX_H

