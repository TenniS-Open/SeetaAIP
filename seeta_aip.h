//
// Created by SeetaTech on 2020/4/16.
//

#ifndef _INC_SEETA_AIP_H
#define _INC_SEETA_AIP_H

#ifdef _MSC_VER
#ifdef SEETA_AIP_EXPORTS
#define SEETA_AIP_API __declspec(dllexport)
#else
#define SEETA_AIP_API __declspec(dllimport)
#endif
#else
#define SEETA_AIP_API __attribute__ ((visibility("default")))
#endif

#define SEETA_AIP_VERSION "1.0"

#ifdef __cplusplus
extern "C" {
#endif

#include <stdint.h>

enum SEETA_AIP_VALUE_TYPE {
    SEETA_AIP_VALUE_BYTE = 0,   ///< byte type
    SEETA_AIP_VALUE_FLOAT = 1,  ///< float type with 4-bytes
    SEETA_AIP_VALUE_INT = 2,    ///< signed integer type with 4-bytes
};

/**
 * \brief Point in image
 * Origin is the left upper corner.
 * X means coordinates from left to right.
 * Y means coordinates from top to bottom.
 */
struct SeetaAIPPoint {
    float x;            ///< X-coordinate
    float y;            ///< Y-coordinate
};


enum SEETA_AIP_SHAPE_TYPE {
    /**
     * Unknown shape
     */
    SEETA_AIP_UNKNOWN_SHAPE = 0,

    /**
     * with forced rotate=0, scale=1, size>=1
     * points represents points
     */
    SEETA_AIP_POINTS = 1,

    /**
     * with forced rotate=0, scale=1, size>=2
     * points represents multi lines:
     *     points[0]->points[1], points[1]->points[2], ..., points[size-2]->points[size-1]
     */
    SEETA_AIP_LINES = 2,

    /**
     * with forced scale=1, size=2
     * rotate represents the angle of rotation around the center point.
     * points[0] represents the left-top corner
     * points[1] represents the right-bottom corner
     */
    SEETA_AIP_RECTANGLE = 3,

    /**
     * with forced rotate=0, scale=1, size=3
     * rotate represents the angle of rotation around the center point.
     * points represents the first 3 points of parallelogram with anticlockwise
     */
    SEETA_AIP_PARALLELOGRAM = 4,

    /**
     * with forced rotate=0, scale=1, size>=2
     * points represents multi lines with anticlockwise:
     *     points[0]->points[1], points[1]->points[2], ...,
     *     points[size-2]->points[size-1], points[size-1]->points[0]
     */
    SEETA_AIP_POLYGON = 5,

    /**
     * with forced rotate=0, size=1
     * scale represents the radius
	 * points[0] represents the center
     */
    SEETA_AIP_CIRCLE = 6,

    /**
     * with forced rotate=0, scale=1, size=3
     * points[0] represents the left-top-front corner
     * points[1] represents the right-bottom-front corner
     * points[2] represents the right-top-back corner
     */
    SEETA_AIP_CUBE = 7,

    /**
     * Means the shape is undefined
     */
    SEETA_AIP_NO_SHAPE = 255,
};

/**
 * \brief shape of object
 */
struct SeetaAIPShape {
    int32_t type;                   ///< SEETA_AIP_SHAPE_TYPE, type of this shape
    struct Landmarks {
        struct SeetaAIPPoint *data; ///< landmarks
        uint32_t size;              ///< size of points
    } landmarks;
    float rotate;                   ///< using degree measure, positive value means anticlockwise
    float scale;                    ///< normally means scale of point's coordinate, specially means radius of a circle
};

/**
 * \brief detectable object
 */
struct SeetaAIPObject {
    struct SeetaAIPShape shape;     ///< shape of object
    struct Tags {
        uint32_t *label;                ///< label index, reserved in SEETAX_PURE_DETECTION mode
        float *score;                   ///< score of each label
        uint32_t size;                  ///< size of label and score
    } tags;                         ///< tags of object
    struct Extra {
        int32_t type;                   ///< SEETA_AIP_VALUE_TYPE, extra data type, most be SEETA_AIP_VALUE_FLOAT
        void *data;                     ///< empty for no extra data
        struct Dims {
            uint32_t *data;                 ///< dims data
            uint32_t size;                  ///< dims size
        } dims;                         ///< dims of extra data
    } extra;                        ///< extra data of object, be related to algorithm
};

/**
 * \brief computing or running device
 */
struct SeetaAIPDevice {
    const char *device;
    int32_t id;
};

/**
 * \brief ImageData type
 */
struct SeetaAIPImageData {
    int32_t type;                   ///< SEETA_AIP_VALUE_TYPE, data value type, support SEETA_AIP_VALUE_BYTE or SEETA_AIP_VALUE_FLOAT
    void *data;                     ///< an array contains each pixel with dims [height, width, channels], the pixel type should be the given type
    // type=SEETA_AIP_VALUE_BYTE represents decltype(*data)=uint8_t
    // type=SEETA_AIP_VALUE_FLOAT represents decltype(*data)=float
    uint32_t number;                ///< number of data
    uint32_t height;                ///< height of image
    uint32_t width;                 ///< width of image
    uint32_t channels;              ///< channels of image
};

struct SeetaAIPStruct;
/**
 * AIP handle
 */
typedef struct SeetaAIPStruct *SeetaAIPHandle;

/**
 * Get readable string for errcode
 * @param [in] errcode non-zero error code, suggest -1 for the last error.
 * @return string for error code description
 */
typedef const char* seeta_aip_error(int32_t errcode);

/**
 * Create the aip
 * @param [out] paip pointer to created
 * @param [in] device NULL for default device
 * @param [in] models C-style of string, end with NULL. Example: {"file1.dat", "file2.json", NULL}
 * @return error code, zero for succeed.
 */
typedef int32_t seeta_aip_create(SeetaAIPHandle *paip,
        const SeetaAIPDevice *device,
        const char **models);

/**
 *
 * @param [in] aip The AIP Handle
 * @return error code, zero for succeed.
 */
typedef int32_t seeta_aip_free(SeetaAIPHandle aip);


/**
 * @param [in] aip The AIP Handle
 * @return error code, zero for succeed.
 */
typedef int32_t seeta_aip_reset(SeetaAIPHandle aip);

/**
 *
 * @param [in] aip The AIP Handle
 * @param [in] method_id
 * @param [in] images
 * @param [in] images_size
 * @param [in] objects
 * @param [in] objects_size
 * @param [out] result_objects
 * @param [out] result_objects_size
 * @param [out] result_images
 * @param [out] result_images_size
 * @return error code, zero for succeed.
 * @note all the return value should be borrowed value, no need to free outside
 */
typedef int32_t seeta_aip_forward(SeetaAIPHandle aip,
                                  uint32_t method_id,
                                  const struct SeetaAIPImageData *images, uint32_t images_size,
                                  const struct SeetaAIPObject *objects, uint32_t objects_size,
                                  struct SeetaAIPObject **result_objects, uint32_t *result_objects_size,
                                  struct SeetaAIPImageData **result_images, uint32_t*result_images_size);

/**
 * @param [in] aip The AIP Handle
 * @param [out] property pointer to int list, contains all property_id, end with 0, like: {1001, 1002, 0}
 * @return error code, zero for succeed.
 * @note the return property must can be saved
 */
typedef int32_t seeta_aip_property(SeetaAIPHandle aip, int32_t **property);

/**
 * @param [in] aip The AIP Handle
 * @param [in] property_id property id
 * @param [in] value set value
 * @return error code, zero for succeed.
 * @note the return property must can be saved
 */
typedef int32_t seeta_aip_set(SeetaAIPHandle aip, int32_t property_id, double value);

/**
 * @param [in] aip The AIP Handle
 * @param [in] property_id property id
 * @param [out] value get value
 * @note the return property must can be saved
 */
typedef int32_t seeta_aip_get(SeetaAIPHandle aip, int32_t property_id, double *value);

#define SEETA_AIP_API_VERSION 1

struct SeetaAIP {
    /**
     * Version for AIP's AIP version. must be SEETA_AIP_API_VERSION for what you seeing.
     * Must be the first member in next versions.
     */
    int32_t aip_version;

    /**
     * String for module name, must be `[a-zA-Z_][a-zA-Z_0-9]*`, used to distinguish from other libraries.
     * For example, SeetaFaceDetector610
     */
    const char *module;

    /**
     * JSON string for more information.
     * Must contain:
     * 1. module files number
     * 2. supported computing device
     * 3. input image format of each method
     * 4. input object format of each method
     * 5. output object format of each method
     * 6. output image format of each method
     * 7. each property description and default value
     */
    const char *description;    ///< json string for more information, the format will post later
    const char *mID;            ///< not readable ID of AIP, only satisfied in system
    const char *sID;            ///< self describable algorithm ID, like SSD, FRCNN etc.
    const char *version;        ///< this AIP's version, comparable `Dotted String`, like 1.3, 6.4.0, or 1.2.3.rc1
    const char **support;       ///< C-stype array of string, like {'cpu', 'gpu', NULL}, only for tips

    seeta_aip_error *error;     ///< used to convert error number to error string
    seeta_aip_create *create;   ///< create a new AIP instance
    seeta_aip_free *free;       ///< free the AIP instance
    seeta_aip_property *property;   ///< list all properties, used to save all property to system
    seeta_aip_get *get;         ///< get AIP's property
    seeta_aip_set *set;         ///< set AIP's property
    seeta_aip_reset *reset;     ///< reset AIP, for video status AIP
    seeta_aip_forward *forward; ///< forward an image, got processed image, detected object or other extra data
};

enum SEETA_AIP_LOAD_ERROR {
    SEETA_AIP_LOAD_SUCCEED = 0,
    SEETA_AIP_LOAD_SIZE_NOT_ENOUGH = 1, ///< once this error return the wanted version will be set.
    SEETA_AIP_LOAD_UNHANDLED_INTERNAL_ERROR = 2,    ///< for unknown load failed, no more informations
};

/**
 *
 * @param aip return loaded AIP
 * @param size must greater than 4, should be sizeof(SeetaAIP)
 * @return SEETA_AIP_LOAD_ERROR
 * For each libraries can be static linked in system, the should be another exported API,
 * named like <module>_aip_load, the <module> is returned aip's module
 */
SEETA_AIP_API int32_t seeta_aip_load(struct SeetaAIP *aip, uint32_t size);

#ifdef __cplusplus
}
#endif

#endif //_INC_SEETA_AIP_H
