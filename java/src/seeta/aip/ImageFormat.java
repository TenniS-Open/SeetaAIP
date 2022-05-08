package seeta.aip;

/**
 * AI Package image format.
 * U8xxx means using byte value type.
 * I32xxx means using integer value type.
 * F32xxx means using float value type.
 */
public enum ImageFormat {
    U8Raw,
    F32Raw,
    I32Raw,
    U8Rgb,
    U8Bgr,
    U8Rgba,
    U8Bgra,
    U8Y,
}
