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
    SEETA_AIP_UNKOWN_SHAPE = 0,
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
};

/**
 * \brief shape of object
 */
struct SeetaAIPShape {
    enum SEETA_AIP_SHAPE_TYPE type; ///< type of this shape
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
        enum SEETA_AIP_VALUE_TYPE type; ///< extra data type
        void *data;                     ///< extra data pointer
        uint32_t size;                  ///< extra data size
    } extra;                        ///< extra data of object, be related to algorithm
};

/**
 * \brief computing or running device
 */
struct SeetaAIPDevice {
    const char *device;
    int32_t id;
};

struct SeetaAIPContainer;

/**
 * \brief ImageData type
 */
struct SeetaAIPImageData {
    enum SEETA_AIP_VALUE_TYPE type;    ///< data value type, support SEETA_AIP_VALUE_BYTE or SEETA_AIP_VALUE_FLOAT
    void *data;                     ///< an array contains each pixel with dims [height, width, channels], the pixel type should be the given type
    // type=SEETA_AIP_VALUE_BYTE represents decltype(*data)=uint8_t
    // type=SEETA_AIP_VALUE_FLOAT represents decltype(*data)=float
    uint32_t height;                ///< height of image
    uint32_t width;                 ///< width of image
    uint32_t channels;              ///< channels of image
};

/**
 * Got an string to describe this AIP's ability
 * @return json string
 */
const char* seeta_aip_describe();

/**
 * Got english error message by errcode
 * @param [in] errcode
 * @return meaningful error message
 */
const char* seeta_api_error_message(int32_t errcode);

int32_t seeta_aip_create(struct SeetaAIPContainer *paip);

int32_t seeta_aip_free(struct SeetaAIPContainer *aip);

int32_t seeta_aip_reset(struct SeetaAIPContainer *aip);

int32_t seeta_aip_forward(struct SeetaAIPContainer *aip,
                uint32_t method_id,
                const struct SeetaAIPImageData *images, uint32_t images_size,
                const struct SeetaAIPImageData *objects, uint32_t objects_size,
                struct SeetaAIPImageData **result_objects, uint32_t *result_size);

#ifdef __cplusplus
}
#endif

#endif //_INC_SEETA_AIP_H
