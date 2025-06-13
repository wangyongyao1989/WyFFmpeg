package com.example.myyffmpeg.converter;

class MediaFormat1 {

    object MIMEType {

        const val MIMETYPE_VIDEO_VP8: String = "video/x-vnd.on2.vp8"

        const val MIMETYPE_VIDEO_VP9: String = "video/x-vnd.on2.vp9"

        const val MIMETYPE_VIDEO_AV1: String = "video/av01"

        const val MIMETYPE_VIDEO_AVC: String = "video/avc"

        const val MIMETYPE_VIDEO_HEVC: String = "video/hevc"

        const val MIMETYPE_VIDEO_MPEG4: String = "video/mp4v-es"

        const val MIMETYPE_VIDEO_H263: String = "video/3gpp"

        const val MIMETYPE_VIDEO_MPEG2: String = "video/mpeg2"

        const val MIMETYPE_VIDEO_RAW: String = "video/raw"

        const val MIMETYPE_VIDEO_DOLBY_VISION: String = "video/dolby-vision"

        const val MIMETYPE_VIDEO_SCRAMBLED: String = "video/scrambled"


        const val MIMETYPE_AUDIO_AMR_NB: String = "audio/3gpp"

        const val MIMETYPE_AUDIO_AMR_WB: String = "audio/amr-wb"

        const val MIMETYPE_AUDIO_MPEG: String = "audio/mpeg"

        const val MIMETYPE_AUDIO_AAC: String = "audio/mp4a-latm"

        const val MIMETYPE_AUDIO_QCELP: String = "audio/qcelp"

        const val MIMETYPE_AUDIO_VORBIS: String = "audio/vorbis"

        const val MIMETYPE_AUDIO_OPUS: String = "audio/opus"

        const val MIMETYPE_AUDIO_G711_ALAW: String = "audio/g711-alaw"

        const val MIMETYPE_AUDIO_G711_MLAW: String = "audio/g711-mlaw"

        const val MIMETYPE_AUDIO_RAW: String = "audio/raw"

        const val MIMETYPE_AUDIO_FLAC: String = "audio/flac"

        const val MIMETYPE_AUDIO_MSGSM: String = "audio/gsm"

        const val MIMETYPE_AUDIO_AC3: String = "audio/ac3"

        const val MIMETYPE_AUDIO_EAC3: String = "audio/eac3"

        const val MIMETYPE_AUDIO_EAC3_JOC: String = "audio/eac3-joc"

        const val MIMETYPE_AUDIO_AC4: String = "audio/ac4"

        const val MIMETYPE_AUDIO_SCRAMBLED: String = "audio/scrambled"
        /** MIME type for MPEG-H Audio single stream  */

        const val MIMETYPE_AUDIO_MPEGH_MHA1: String = "audio/mha1"
        /** MIME type for MPEG-H Audio single stream, encapsulated in MHAS  */

        const val MIMETYPE_AUDIO_MPEGH_MHM1: String = "audio/mhm1"
        /** MIME type for DTS Digital Surround (up to 5.1 channels) audio stream, aka DTS-CA.  */

        const val MIMETYPE_AUDIO_DTS: String = "audio/vnd.dts"
        /**
         * MIME type for DTS HD (up to 7.1 channels) audio stream.
         * With codec profile DTS_HDProfileHRA represents DTS HD High Resolution Audio.
         * With codec profile DTS_HDProfileMA represents DTS HD Master Audio.
         * With codec profile DTS_HDProfileLBR represents DTS Express.
         */

        const val MIMETYPE_AUDIO_DTS_HD: String = "audio/vnd.dts.hd"
        /**
         * MIME type for DTS UHD (object-based) audio stream, aka DTS:X.
         * With codec profile DTS_UHDProfileP1 represents DTS-UHD P1.
         * With codec profile DTS_UHDProfileP2 represents DTS-UHD P2.
         */

        const val MIMETYPE_AUDIO_DTS_UHD: String = "audio/vnd.dts.uhd"
        /** MIME type for Dynamic Resolution Adaptation (DRA) audio stream.  */

        const val MIMETYPE_AUDIO_DRA: String = "audio/vnd.dra"
        /** MIME type for Dolby Metadata-enhanced Audio Transmission (MAT) audio stream.  */

        const val MIMETYPE_AUDIO_DOLBY_MAT: String = "audio/vnd.dolby.mat"
        /** MIME type for Dolby TrueHD audio format, based on Meridian Lossless Packing (MLP).  */

        const val MIMETYPE_AUDIO_DOLBY_TRUEHD: String = "audio/vnd.dolby.mlp"
        /**
         * MIME type for AAC Low Complexity (LC) audio stream. Uses the scheme defined by
         * RFC 6381 with OTI of MPEG-4 (40) and AOT of AAC LC (2) from ISO/IEC 14496-3.
         */

        const val MIMETYPE_AUDIO_AAC_LC: String = "audio/mp4a.40.02"
        /**
         * MIME type for HE-AAC v1 (LC + SBR) audio stream. Uses the scheme defined by
         * RFC 6381 with OTI of MPEG-4 (40) and AOT of AAC SBR (5) from ISO/IEC 14496-3.
         */

        const val MIMETYPE_AUDIO_AAC_HE_V1: String = "audio/mp4a.40.05"
        /**
         * MIME type for HE-AAC v2 (LC + SBR + PS) audio stream. Uses the scheme defined by
         * RFC 6381 with OTI of MPEG-4 (40) and AOT of PS (29) from ISO/IEC 14496-3.
         */

        const val MIMETYPE_AUDIO_AAC_HE_V2: String = "audio/mp4a.40.29"
        /**
         * MIME type for AAC Enhanced Low Delay (ELD) audio stream. Uses the scheme defined by
         * RFC 6381 with OTI of MPEG-4 (40) and AOT of ELD (39) from ISO/IEC 14496-3.
         */

        const val MIMETYPE_AUDIO_AAC_ELD: String = "audio/mp4a.40.39"
        /**
         * MIME type for AAC XHE audio stream. Uses the scheme defined by
         * RFC 6381 with OTI of MPEG-4 (40) and AOT of USAC (42) from ISO/IEC 14496-3.
         */

        const val MIMETYPE_AUDIO_AAC_XHE: String = "audio/mp4a.40.42"
        /**
         * MIME type for MPEG-H Baseline (BL) Profile L3 audio stream. Uses the scheme defined by
         * RFC 6381 with mpegh3daProfileLevelIndication for main profile/L3 (0x3) from ISO/IEC 23008-3.
         */

        const val MIMETYPE_AUDIO_MPEGH_BL_L3: String = "audio/mhm1.03"
        /**
         * MIME type for MPEG-H Baseline (BL) Profile L4 audio stream. Uses the scheme defined by
         * RFC 6381 with mpegh3daProfileLevelIndication for main profile/L4 (0x4) from ISO/IEC 23008-3.
         */

        const val MIMETYPE_AUDIO_MPEGH_BL_L4: String = "audio/mhm1.04"
        /**
         * MIME type for MPEG-H Low Complexity (LC) L3 audio stream. Uses the scheme defined by
         * RFC 6381 with mpegh3daProfileLevelIndication for LC profile/L3 (0xD) from ISO/IEC 23008-3.
         */

        const val MIMETYPE_AUDIO_MPEGH_LC_L3: String = "audio/mhm1.0d"
        /**
         * MIME type for MPEG-H Low Complexity (LC) L4 audio stream. Uses the scheme defined by
         * RFC 6381 with mpegh3daProfileLevelIndication for LC profile/L4 (0xE) from ISO/IEC 23008-3.
         */

        const val MIMETYPE_AUDIO_MPEGH_LC_L4: String = "audio/mhm1.0e"
        /**
         * MIME type for the IEC61937 audio stream encapsulation. This type isn't defined by IANA.
         */

        const val MIMETYPE_AUDIO_IEC61937: String = "audio/x-iec61937"

        /**
         * MIME type for HEIF still image data encoded in HEVC.
         *
         * To decode such an image, [MediaCodec] decoder for
         * [.MIMETYPE_VIDEO_HEVC] shall be used. The client needs to form
         * the correct [.MediaFormat] based on additional information in
         * the track format (shown in the next paragraph), and send it to
         * [MediaCodec.configure].
         *
         * The track's MediaFormat will come with [.WIDTH] and
         * [.HEIGHT] keys, which describes the width and height
         * of the image. If the image doesn't contain grid (i.e. none of
         * [.TILE_WIDTH], [.TILE_HEIGHT],
         * [.GRID_ROWS], [.GRID_COLUMNS] are present), the
         * track will contain a single sample of coded data for the entire image,
         * and the image width and height should be used to set up the decoder.
         *
         * If the image does come with grid, each sample from the track will
         * contain one tile in the grid, of which the size is described by
         * [.TILE_WIDTH] and [.TILE_HEIGHT]. This size
         * (instead of [.WIDTH] and [.HEIGHT]) should be
         * used to set up the decoder. The track contains [.GRID_ROWS]
         * by [.GRID_COLUMNS] samples in row-major, top-row first,
         * left-to-right order. The output image should be reconstructed by
         * first tiling the decoding results of the tiles in the correct order,
         * then trimming (before rotation is applied) on the bottom and right
         * side, if the tiled area is larger than the image width and height.
         */

        const val MIMETYPE_IMAGE_ANDROID_HEIC: String = "image/vnd.android.heic"

        /**
         * MIME type for AVIF still image data encoded in AV1.
         *
         * To decode such an image, [MediaCodec] decoder for
         * [.MIMETYPE_VIDEO_AV1] shall be used. The client needs to form
         * the correct [.MediaFormat] based on additional information in
         * the track format (shown in the next paragraph), and send it to
         * [MediaCodec.configure].
         *
         * The track's MediaFormat will come with [.WIDTH] and
         * [.HEIGHT] keys, which describes the width and height
         * of the image. If the image doesn't contain grid (i.e. none of
         * [.TILE_WIDTH], [.TILE_HEIGHT],
         * [.GRID_ROWS], [.GRID_COLUMNS] are present), the
         * track will contain a single sample of coded data for the entire image,
         * and the image width and height should be used to set up the decoder.
         *
         * If the image does come with grid, each sample from the track will
         * contain one tile in the grid, of which the size is described by
         * [.TILE_WIDTH] and [.TILE_HEIGHT]. This size
         * (instead of [.WIDTH] and [.HEIGHT]) should be
         * used to set up the decoder. The track contains [.GRID_ROWS]
         * by [.GRID_COLUMNS] samples in row-major, top-row first,
         * left-to-right order. The output image should be reconstructed by
         * first tiling the decoding results of the tiles in the correct order,
         * then trimming (before rotation is applied) on the bottom and right
         * side, if the tiled area is larger than the image width and height.
         */

        const val MIMETYPE_IMAGE_AVIF: String = "image/avif"

        /**
         * MIME type for WebVTT subtitle data.
         */

        const val MIMETYPE_TEXT_VTT: String = "text/vtt"

        /**
         * MIME type for SubRip (SRT) container.
         */

        const val MIMETYPE_TEXT_SUBRIP: String = "application/x-subrip"

        /**
         * MIME type for CEA-608 closed caption data.
         */

        const val MIMETYPE_TEXT_CEA_608: String = "text/cea-608"

        /**
         * MIME type for CEA-708 closed caption data.
         */

        const val MIMETYPE_TEXT_CEA_708: String = "text/cea-708"
    }

    object KEYS {
        /**
         * A key describing the mime type of the MediaFormat.
         * The associated value is a string.
         */

        const val MIME: String = "mime"

        /**
         * A key describing the codecs string of the MediaFormat. See RFC 6381 section 3.2 for the
         * syntax of the value. The value does not hold [MediaCodec]-exposed codec names.
         * The associated value is a string.
         *
         * @see MediaParser.TrackData.mediaFormat
         */

        const val CODECS_STRING: String = "codecs-string"

        /**
         * An optional key describing the low latency decoding mode. This is an optional parameter
         * that applies only to decoders. If enabled, the decoder doesn't hold input and output
         * data more than required by the codec standards.
         * The associated value is an integer (0 or 1): 1 when low-latency decoding is enabled,
         * 0 otherwise. The default value is 0.
         */

        const val LOW_LATENCY: String = "low-latency"

        /**
         * A key describing the language of the content, using either ISO 639-1
         * or 639-2/T codes.  The associated value is a string.
         */

        const val LANGUAGE: String = "language"

        /**
         * A key describing the closed caption service number. For CEA-608 caption tracks, holds the
         * channel number. For CEA-708, holds the service number.
         * The associated value is an int.
         */

        const val CAPTION_SERVICE_NUMBER: String = "caption-service-number"

        /**
         * A key describing the sample rate of an audio format.
         * The associated value is an integer
         */

        const val SAMPLE_RATE: String = "sample-rate"

        /**
         * A key describing the number of channels in an audio format.
         * The associated value is an integer
         */

        const val CHANNEL_COUNT: String = "channel-count"

        /**
         * A key describing the width of the content in a video format.
         * The associated value is an integer
         */

        const val WIDTH: String = "width"

        /**
         * A key describing the height of the content in a video format.
         * The associated value is an integer
         */

        const val HEIGHT: String = "height"

        /**
         * A key describing the bottom-coordinate (y) of the crop rectangle.
         * This is the bottom-most row included in the crop frame,
         * where row indices start at 0.
         * Additional information on the crop rectangle semantics can be found at
         * [android.media.MediaCodec].
         */

        const val CROP_BOTTOM: String = "crop-bottom"

        /**
         * A key describing the left-coordinate (x) of the crop rectangle.
         * This is the left-most column included in the crop frame,
         * where column indices start at 0.
         * Additional information on the crop rectangle semantics can be found at
         * [android.media.MediaCodec].
         */

        const val CROP_LEFT: String = "crop-left"

        /**
         * A key describing the right-coordinate (x) of the crop rectangle.
         * This is the right-most column included in the crop frame,
         * where column indices start at 0.
         * Additional information on the crop rectangle semantics can be found at
         * [android.media.MediaCodec].
         */

        const val CROP_RIGHT: String = "crop-right"

        /**
         * A key describing the top-coordinate (y) of the crop rectangle.
         * This is the top-most row included in the crop frame,
         * where row indices start at 0.
         * Additional information on the crop rectangle semantics can be found at
         * [android.media.MediaCodec].
         */

        const val CROP_TOP: String = "crop-top"

        /**
         * A key describing the maximum expected width of the content in a video
         * decoder format, in case there are resolution changes in the video content.
         * The associated value is an integer
         */

        const val MAX_WIDTH: String = "max-width"

        /**
         * A key describing the maximum expected height of the content in a video
         * decoder format, in case there are resolution changes in the video content.
         * The associated value is an integer
         */

        const val MAX_HEIGHT: String = "max-height"

        /** A key describing the maximum size in bytes of a buffer of data
         * described by this MediaFormat.
         * The associated value is an integer
         */

        const val MAX_INPUT_SIZE: String = "max-input-size"

        /**
         * A key describing the pixel aspect ratio width.
         * The associated value is an integer
         */

        const val PIXEL_ASPECT_RATIO_WIDTH: String = "sar-width"

        /**
         * A key describing the pixel aspect ratio height.
         * The associated value is an integer
         */

        const val PIXEL_ASPECT_RATIO_HEIGHT: String = "sar-height"

        /**
         * A key describing the average bitrate in bits/sec.
         * The associated value is an integer
         */

        const val BIT_RATE: String = "bitrate"

        /**
         * A key describing the hardware AV sync id.
         * The associated value is an integer
         *
         * See android.media.tv.tuner.Tuner#getAvSyncHwId.
         */

        const val HARDWARE_AV_SYNC_ID: String = "hw-av-sync-id"

        /**
         * A key describing the max bitrate in bits/sec.
         * This is usually over a one-second sliding window (e.g. over any window of one second).
         * The associated value is an integer
         * @hide
         */

        const val MAX_BIT_RATE: String = "max-bitrate"

        /**
         * A key describing the color format of the content in a video format.
         * Constants are declared in [android.media.MediaCodecInfo.CodecCapabilities].
         */

        const val COLOR_FORMAT: String = "color-format"

        /**
         * A key describing the frame rate of a video format in frames/sec.
         *
         *
         * The associated value is normally an integer when the value is used by the platform,
         * but video codecs also accept float configuration values.
         * Specifically, [MediaExtractor][MediaExtractor.getTrackFormat] provides an integer
         * value corresponding to the frame rate information of the track if specified and non-zero.
         * Otherwise, this key is not present. [MediaCodec][MediaCodec.configure] accepts both
         * float and integer values.
         *
         *
         * This represents the desired operating frame rate if the
         * [.OPERATING_RATE] is not present and [.PRIORITY] is `0`
         * (realtime). Otherwise, this is just informational.
         *
         *
         * For video encoders this value corresponds to the intended frame rate (the rate at which
         * the application intends to send frames to the encoder, as calculated by the buffer
         * timestamps, and not from the actual real-time rate that the frames are sent to
         * the encoder). Encoders use this hint for rate control, specifically for the initial
         * frames, as encoders are expected to support variable frame rate (for rate control) based
         * on the actual [buffer timestamps][MediaCodec.BufferInfo.presentationTimeUs] of
         * subsequent frames.
         *
         *
         * This key is not used in the `MediaCodec`
         * [input][MediaCodec.getInputFormat]/[output][MediaCodec.getOutputFormat] formats,
         * nor by [MediaMuxer][MediaMuxer.addTrack].
         */

        const val FRAME_RATE: String = "frame-rate"

        /**
         * A key describing the width (in pixels) of each tile of the content in a
         * [.MIMETYPE_IMAGE_ANDROID_HEIC] / [.MIMETYPE_IMAGE_AVIF] track.
         * The associated value is an integer.
         *
         * Refer to [.MIMETYPE_IMAGE_ANDROID_HEIC] / [.MIMETYPE_IMAGE_AVIF] on decoding
         * instructions of such tracks.
         *
         * @see .TILE_HEIGHT
         *
         * @see .GRID_ROWS
         *
         * @see .GRID_COLUMNS
         */

        const val TILE_WIDTH: String = "tile-width"

        /**
         * A key describing the height (in pixels) of each tile of the content in a
         * [.MIMETYPE_IMAGE_ANDROID_HEIC] / [.MIMETYPE_IMAGE_AVIF] track.
         * The associated value is an integer.
         *
         * Refer to [.MIMETYPE_IMAGE_ANDROID_HEIC] / [.MIMETYPE_IMAGE_AVIF] on decoding
         * instructions of such tracks.
         *
         * @see .TILE_WIDTH
         *
         * @see .GRID_ROWS
         *
         * @see .GRID_COLUMNS
         */

        const val TILE_HEIGHT: String = "tile-height"

        /**
         * A key describing the number of grid rows in the content in a
         * [.MIMETYPE_IMAGE_ANDROID_HEIC] / [.MIMETYPE_IMAGE_AVIF] track.
         * The associated value is an integer.
         *
         * Refer to [.MIMETYPE_IMAGE_ANDROID_HEIC] / [.MIMETYPE_IMAGE_AVIF] on decoding
         * instructions of such tracks.
         *
         * @see .TILE_WIDTH
         *
         * @see .TILE_HEIGHT
         *
         * @see .GRID_COLUMNS
         */

        const val GRID_ROWS: String = "grid-rows"

        /**
         * A key describing the number of grid columns in the content in a
         * [.MIMETYPE_IMAGE_ANDROID_HEIC] / [.MIMETYPE_IMAGE_AVIF] track.
         * The associated value is an integer.
         *
         * Refer to [.MIMETYPE_IMAGE_ANDROID_HEIC] / [.MIMETYPE_IMAGE_AVIF] on decoding
         * instructions of such tracks.
         *
         * @see .TILE_WIDTH
         *
         * @see .TILE_HEIGHT
         *
         * @see .GRID_ROWS
         */

        const val GRID_COLUMNS: String = "grid-cols"

        /**
         * A key describing the raw audio sample encoding/format.
         *
         *
         * The associated value is an integer, using one of the
         * [AudioFormat].ENCODING_PCM_ values.
         *
         *
         * This is an optional key for audio decoders and encoders specifying the
         * desired raw audio sample format during [ MediaCodec.configure(&amp;hellip;)][MediaCodec.configure] call. Use [ MediaCodec.getInput][MediaCodec.getInputFormat]/[OutputFormat(&amp;hellip;)][MediaCodec.getOutputFormat]
         * to confirm the actual format. For the PCM decoder this key specifies both
         * input and output sample encodings.
         *
         *
         * This key is also used by [MediaExtractor] to specify the sample
         * format of audio data, if it is specified.
         *
         *
         * If this key is missing, the raw audio sample format is signed 16-bit short.
         */

        const val PCM_ENCODING: String = "pcm-encoding"

        /**
         * A key describing the capture rate of a video format in frames/sec.
         *
         *
         * When capture rate is different than the frame rate, it means that the
         * video is acquired at a different rate than the playback, which produces
         * slow motion or timelapse effect during playback. Application can use the
         * value of this key to tell the relative speed ratio between capture and
         * playback rates when the video was recorded.
         *
         *
         *
         * The associated value is an integer or a float.
         *
         */

        const val CAPTURE_RATE: String = "capture-rate"

        /**
         * A key for retrieving the slow-motion marker information associated with a video track.
         *
         *
         * The associated value is a ByteBuffer in [ByteOrder.BIG_ENDIAN]
         * (networking order) of the following format:
         *
         * <pre class="prettyprint">
         * float(32) playbackRate;
         * unsigned int(32) numMarkers;
         * for (i = 0;i < numMarkers; i++) {
         * int(64) timestampUs;
         * float(32) speedRatio;
         * }</pre>
         * The meaning of each field is as follows:
         * <table border="1" width="90%" align="center" cellpadding="5">
         * <tbody>
         * <tr>
         * <td>playbackRate</td>
         * <td>The frame rate at which the playback should happen (or the flattened
         * clip should be).</td>
        </tr> *
         * <tr>
         * <td>numMarkers</td>
         * <td>The number of slow-motion markers that follows.</td>
        </tr> *
         * <tr>
         * <td>timestampUs</td>
         * <td>The starting point of a new segment.</td>
        </tr> *
         * <tr>
         * <td>speedRatio</td>
         * <td>The playback speed for that segment. The playback speed is a floating
         * point number, indicating how fast the time progresses relative to that
         * written in the container. (Eg. 4.0 means time goes 4x as fast, which
         * makes 30fps become 120fps.)</td>
        </tr> *
        </tbody></table> *
         *
         *
         * The following constraints apply to the timestampUs of the markers:
         *
         *  * The timestampUs shall be monotonically increasing.
         *  * The timestampUs shall fall within the time span of the video track.
         *  * The first timestampUs should match that of the first video sample.
         */

        const val SLOW_MOTION_MARKERS: String = "slow-motion-markers"

        /**
         * A key describing the frequency of key frames expressed in seconds between key frames.
         *
         *
         * This key is used by video encoders.
         * A negative value means no key frames are requested after the first frame.
         * A zero value means a stream containing all key frames is requested.
         *
         *
         * Most video encoders will convert this value of the number of non-key-frames between
         * key-frames, using the [frame rate][.FRAME_RATE] information; therefore,
         * if the actual frame rate differs (e.g. input frames are dropped or the frame rate
         * changes), the **time interval** between key frames will not be the
         * configured value.
         *
         *
         * The associated value is an integer (or float since
         * [android.os.Build.VERSION_CODES.N_MR1]).
         */

        const val I_FRAME_INTERVAL: String = "i-frame-interval"

        /**
         * An optional key describing the period of intra refresh in frames. This is an
         * optional parameter that applies only to video encoders. If encoder supports it
         * ([MediaCodecInfo.CodecCapabilities.FEATURE_IntraRefresh]), the whole
         * frame is completely refreshed after the specified period. Also for each frame,
         * a fix subset of macroblocks must be intra coded which leads to more constant bitrate
         * than inserting a key frame. This key is recommended for video streaming applications
         * as it provides low-delay and good error-resilience. This key is ignored if the
         * video encoder does not support the intra refresh feature. Use the output format to
         * verify that this feature was enabled.
         * The associated value is an integer.
         */

        const val INTRA_REFRESH_PERIOD: String = "intra-refresh-period"

        /**
         * An optional key describing whether encoders prepend headers to sync frames (e.g.
         * SPS and PPS to IDR frames for H.264). This is an optional parameter that applies only
         * to video encoders. A video encoder may not support this feature; the component will fail
         * to configure in that case. For other components, this key is ignored.
         *
         * The value is an integer, with 1 indicating to prepend headers to every sync frames,
         * or 0 otherwise. The default value is 0.
         */

        const val PREPEND_HEADER_TO_SYNC_FRAMES: String = "prepend-sps-pps-to-idr-frames"

        /**
         * A key describing the temporal layering schema.  This is an optional parameter
         * that applies only to video encoders.  Use [MediaCodec.getOutputFormat]
         * after [configure][MediaCodec.configure] to query if the encoder supports
         * the desired schema. Supported values are `webrtc.vp8.N-layer`,
         * `android.generic.N`, `android.generic.N+M` and `none`, where
         * `N` denotes the total number of non-bidirectional layers (which must be at least 1)
         * and `M` denotes the total number of bidirectional layers (which must be non-negative).
         *
         * `android.generic.*` schemas have been added in [ ][android.os.Build.VERSION_CODES.N_MR1].
         *
         *
         * The encoder may support fewer temporal layers, in which case the output format
         * will contain the configured schema. If the encoder does not support temporal
         * layering, the output format will not have an entry with this key.
         * The associated value is a string.
         */

        const val TEMPORAL_LAYERING: String = "ts-schema"

        /**
         * A key describing the stride of the video bytebuffer layout.
         * Stride (or row increment) is the difference between the index of a pixel
         * and that of the pixel directly underneath. For YUV 420 formats, the
         * stride corresponds to the Y plane; the stride of the U and V planes can
         * be calculated based on the color format, though it is generally undefined
         * and depends on the device and release.
         * The associated value is an integer, representing number of bytes.
         */

        const val STRIDE: String = "stride"

        /**
         * A key describing the plane height of a multi-planar (YUV) video bytebuffer layout.
         * Slice height (or plane height/vertical stride) is the number of rows that must be skipped
         * to get from the top of the Y plane to the top of the U plane in the bytebuffer. In essence
         * the offset of the U plane is sliceHeight * stride. The height of the U/V planes
         * can be calculated based on the color format, though it is generally undefined
         * and depends on the device and release.
         * The associated value is an integer, representing number of rows.
         */

        const val SLICE_HEIGHT: String = "slice-height"

        /**
         * Applies only when configuring a video encoder in "surface-input" mode.
         * The associated value is a long and gives the time in microseconds
         * after which the frame previously submitted to the encoder will be
         * repeated (once) if no new frame became available since.
         */

        const val REPEAT_PREVIOUS_FRAME_AFTER
                : String = "repeat-previous-frame-after"

        /**
         * Instruct the video encoder in "surface-input" mode to drop excessive
         * frames from the source, so that the input frame rate to the encoder
         * does not exceed the specified fps.
         *
         * The associated value is a float, representing the max frame rate to
         * feed the encoder at.
         *
         */

        const val MAX_FPS_TO_ENCODER
                : String = "max-fps-to-encoder"

        /**
         * Instruct the video encoder in "surface-input" mode to limit the gap of
         * timestamp between any two adjacent frames fed to the encoder to the
         * specified amount (in micro-second).
         *
         * The associated value is a long int. When positive, it represents the max
         * timestamp gap between two adjacent frames fed to the encoder. When negative,
         * the absolute value represents a fixed timestamp gap between any two adjacent
         * frames fed to the encoder. Note that this will also apply even when the
         * original timestamp goes backward in time. Under normal conditions, such frames
         * would be dropped and not sent to the encoder.
         *
         * The output timestamp will be restored to the original timestamp and will
         * not be affected.
         *
         * This is used in some special scenarios where input frames arrive sparingly
         * but it's undesirable to allocate more bits to any single frame, or when it's
         * important to ensure all frames are captured (rather than captured in the
         * correct order).
         *
         */

        const val MAX_PTS_GAP_TO_ENCODER
                : String = "max-pts-gap-to-encoder"

        /**
         * If specified when configuring a video encoder that's in "surface-input"
         * mode, it will instruct the encoder to put the surface source in suspended
         * state when it's connected. No video frames will be accepted until a resume
         * operation (see [MediaCodec.PARAMETER_SUSPEND]), optionally with
         * timestamp specified via [MediaCodec.PARAMETER_SUSPEND_TIME], is
         * received.
         *
         * The value is an integer, with 1 indicating to create with the surface
         * source suspended, or 0 otherwise. The default value is 0.
         *
         * If this key is not set or set to 0, the surface source will accept buffers
         * as soon as it's connected to the encoder (although they may not be encoded
         * immediately). This key can be used when the client wants to prepare the
         * encoder session in advance, but do not want to accept buffers immediately.
         */

        const val CREATE_INPUT_SURFACE_SUSPENDED
                : String = "create-input-buffers-suspended"

        /**
         * If specified when configuring a video decoder rendering to a surface,
         * causes the decoder to output "blank", i.e. black frames to the surface
         * when stopped to clear out any previously displayed contents.
         * The associated value is an integer of value 1.
         */

        const val PUSH_BLANK_BUFFERS_ON_STOP
                : String = "push-blank-buffers-on-shutdown"

        /**
         * A key describing the duration (in microseconds) of the content.
         * The associated value is a long.
         */

        const val DURATION: String = "durationUs"

        /**
         * A key mapping to a value of 1 if the content is AAC audio and
         * audio frames are prefixed with an ADTS header.
         * The associated value is an integer (0 or 1).
         * This key is only supported when _decoding_ content, it cannot
         * be used to configure an encoder to emit ADTS output.
         */

        const val IS_ADTS: String = "is-adts"

        /**
         * A key describing the channel composition of audio content. This mask
         * is composed of bits drawn from channel mask definitions in [android.media.AudioFormat].
         * The associated value is an integer.
         */

        const val CHANNEL_MASK: String = "channel-mask"

        /**
         * A key describing the maximum number of channels that can be output by an audio decoder.
         * By default, the decoder will output the same number of channels as present in the encoded
         * stream, if supported. Set this value to limit the number of output channels, and use
         * the downmix information in the stream, if available.
         *
         * Values larger than the number of channels in the content to decode behave like the number
         * of channels in the content (if applicable), for instance passing 99 for a 5.1 audio stream
         * behaves like passing 6.
         *
         * This key is only used during decoding.
         */

        const val MAX_OUTPUT_CHANNEL_COUNT: String = "max-output-channel-count"

        /**
         * A key describing the number of frames to trim from the start of the decoded audio stream.
         * The associated value is an integer.
         */

        const val ENCODER_DELAY: String = "encoder-delay"

        /**
         * A key describing the number of frames to trim from the end of the decoded audio stream.
         * The associated value is an integer.
         */

        const val ENCODER_PADDING: String = "encoder-padding"

        /**
         * A key describing the AAC profile to be used (AAC audio formats only).
         * Constants are declared in [android.media.MediaCodecInfo.CodecProfileLevel].
         */

        const val AAC_PROFILE: String = "aac-profile"

        /**
         * A key describing the AAC SBR mode to be used (AAC audio formats only).
         * The associated value is an integer and can be set to following values:
         *
         *  * 0 - no SBR should be applied
         *  * 1 - single rate SBR
         *  * 2 - double rate SBR
         *
         * Note: If this key is not defined the default SRB mode for the desired AAC profile will
         * be used.
         *
         * This key is only used during encoding.
         */

        const val AAC_SBR_MODE: String = "aac-sbr-mode"

        /**
         * A key describing the maximum number of channels that can be output by the AAC decoder.
         * By default, the decoder will output the same number of channels as present in the encoded
         * stream, if supported. Set this value to limit the number of output channels, and use
         * the downmix information in the stream, if available.
         *
         * Values larger than the number of channels in the content to decode behave just
         * like the actual channel count of the content (e.g. passing 99 for the decoding of 5.1 content
         * will behave like using 6).
         *
         * This key is only used during decoding.
         */
        @Deprecated("Use the non-AAC-specific key {@link #MAX_OUTPUT_CHANNEL_COUNT} instead")

        const val AAC_MAX_OUTPUT_CHANNEL_COUNT: String = "aac-max-output-channel_count"

        /**
         * A key describing the Target Reference Level (Target Loudness).
         *
         * For normalizing loudness across program items, a gain is applied to the audio output so
         * that the output loudness matches the Target Reference Level. The gain is derived as the
         * difference between the Target Reference Level and the Program Reference Level (Program
         * Loudness). The latter can be given in the bitstream and indicates the actual loudness value
         * of the program item.
         *
         * The Target Reference Level controls loudness normalization for both MPEG-4 DRC and
         * MPEG-D DRC.
         *
         * The value is given as an integer value between
         * 40 and 127, and is calculated as -4 * Target Reference Level in LKFS.
         * Therefore, it represents the range of -10 to -31.75 LKFS.
         *
         * For MPEG-4 DRC, a value of -1 switches off loudness normalization and DRC processing.
         *
         * For MPEG-D DRC, a value of -1 switches off loudness normalization only. For DRC processing
         * options of MPEG-D DRC, see [.AAC_DRC_EFFECT_TYPE]
         *
         * The default value on mobile devices is 64 (-16 LKFS).
         *
         * This key is only used during decoding.
         */

        const val AAC_DRC_TARGET_REFERENCE_LEVEL: String = "aac-target-ref-level"

        /**
         * A key describing for selecting the DRC effect type for MPEG-D DRC.
         * The supported values are defined in ISO/IEC 23003-4:2015 and are described as follows:
         * <table>
         * <tr><th>Value</th><th>Effect</th></tr>
         * <tr><th>-1</th><th>Off</th></tr>
         * <tr><th>0</th><th>None</th></tr>
         * <tr><th>1</th><th>Late night</th></tr>
         * <tr><th>2</th><th>Noisy environment</th></tr>
         * <tr><th>3</th><th>Limited playback range</th></tr>
         * <tr><th>4</th><th>Low playback level</th></tr>
         * <tr><th>5</th><th>Dialog enhancement</th></tr>
         * <tr><th>6</th><th>General compression</th></tr>
        </table> *
         *
         * The value -1 (Off) disables DRC processing, while loudness normalization may still be
         * active and dependent on [.AAC_DRC_TARGET_REFERENCE_LEVEL].<br></br>
         * The value 0 (None) automatically enables DRC processing if necessary to prevent signal
         * clipping<br></br>
         * The value 6 (General compression) can be used for enabling MPEG-D DRC without particular
         * DRC effect type request.<br></br>
         * The default DRC effect type is 3 ("Limited playback range") on mobile devices.
         *
         * This key is only used during decoding.
         */

        const val AAC_DRC_EFFECT_TYPE: String = "aac-drc-effect-type"

        /**
         * A key describing the target reference level that was assumed at the encoder for
         * calculation of attenuation gains for clipping prevention.
         *
         * If it is known, this information can be provided as an integer value between
         * 0 and 127, which is calculated as -4 * Encoded Target Level in LKFS.
         * If the Encoded Target Level is unknown, the value can be set to -1.
         *
         * The default value is -1 (unknown).
         *
         * The value is ignored when heavy compression (see [.AAC_DRC_HEAVY_COMPRESSION])
         * or MPEG-D DRC is used.
         *
         * This key is only used during decoding.
         */

        const val AAC_ENCODED_TARGET_LEVEL: String = "aac-encoded-target-level"

        /**
         * A key describing the boost factor allowing to adapt the dynamics of the output to the
         * actual listening requirements. This relies on DRC gain sequences that can be transmitted in
         * the encoded bitstream to be able to reduce the dynamics of the output signal upon request.
         * This factor enables the user to select how much of the gains are applied.
         *
         * Positive gains (boost) and negative gains (attenuation, see
         * [.AAC_DRC_ATTENUATION_FACTOR]) can be controlled separately for a better match
         * to different use-cases.
         *
         * Typically, attenuation gains are sent for loud signal segments, and boost gains are sent
         * for soft signal segments. If the output is listened to in a noisy environment, for example,
         * the boost factor is used to enable the positive gains, i.e. to amplify soft signal segments
         * beyond the noise floor. But for listening late at night, the attenuation
         * factor is used to enable the negative gains, to prevent loud signal from surprising
         * the listener. In applications which generally need a low dynamic range, both the boost factor
         * and the attenuation factor are used in order to enable all DRC gains.
         *
         * In order to prevent clipping, it is also recommended to apply the attenuation gains
         * in case of a downmix and/or loudness normalization to high target reference levels.
         *
         * Both the boost and the attenuation factor parameters are given as integer values
         * between 0 and 127, representing the range of the factor of 0 (i.e. don't apply)
         * to 1 (i.e. fully apply boost/attenuation gains respectively).
         *
         * The default value is 127 (fully apply boost DRC gains).
         *
         * This key is only used during decoding.
         */

        const val AAC_DRC_BOOST_FACTOR: String = "aac-drc-boost-level"

        /**
         * A key describing the attenuation factor allowing to adapt the dynamics of the output to the
         * actual listening requirements.
         * See [.AAC_DRC_BOOST_FACTOR] for a description of the role of this attenuation
         * factor and the value range.
         *
         * The default value is 127 (fully apply attenuation DRC gains).
         *
         * This key is only used during decoding.
         */

        const val AAC_DRC_ATTENUATION_FACTOR: String = "aac-drc-cut-level"

        /**
         * A key describing the selection of the heavy compression profile for MPEG-4 DRC.
         *
         * Two separate DRC gain sequences can be transmitted in one bitstream: light compression
         * and heavy compression. When selecting the application of the heavy compression, one of
         * the sequences is selected:
         *
         *  * 0 enables light compression,
         *  * 1 enables heavy compression instead.
         *
         * Note that heavy compression doesn't offer the features of scaling of DRC gains
         * (see [.AAC_DRC_BOOST_FACTOR] and [.AAC_DRC_ATTENUATION_FACTOR] for the
         * boost and attenuation factors), and frequency-selective (multiband) DRC.
         * Light compression usually contains clipping prevention for stereo downmixing while heavy
         * compression, if additionally provided in the bitstream, is usually stronger, and contains
         * clipping prevention for stereo and mono downmixing.
         *
         * The default is 1 (heavy compression).
         *
         * This key is only used during decoding.
         */

        const val AAC_DRC_HEAVY_COMPRESSION: String = "aac-drc-heavy-compression"

        /**
         * A key to retrieve the output loudness of a decoded bitstream.
         *
         * If loudness normalization is active, the value corresponds to the Target Reference Level
         * (see [.AAC_DRC_TARGET_REFERENCE_LEVEL]).<br></br>
         * If loudness normalization is not active, the value corresponds to the loudness metadata
         * given in the bitstream.
         *
         * The value is retrieved with getInteger() and is given as an integer value between 0 and
         * 231. It is calculated as -4 * Output Loudness in LKFS. Therefore, it represents the range of
         * 0 to -57.75 LKFS.
         *
         * A value of -1 indicates that no loudness metadata is present in the bitstream.
         *
         * Loudness metadata can originate from MPEG-4 DRC or MPEG-D DRC.
         *
         * This key is only used during decoding.
         */

        const val AAC_DRC_OUTPUT_LOUDNESS: String = "aac-drc-output-loudness"

        /**
         * A key describing the album mode for MPEG-D DRC as defined in ISO/IEC 23003-4.
         *
         * The associated value is an integer and can be set to following values:
         * <table>
         * <tr><th>Value</th><th>Album Mode</th></tr>
         * <tr><th>0</th><th>disabled</th></tr>
         * <tr><th>1</th><th>enabled</th></tr>
        </table> *
         *
         * Disabled album mode leads to application of gain sequences for fading in and out, if
         * provided in the bitstream. Enabled album mode makes use of dedicated album loudness
         * information, if provided in the bitstream.
         *
         * The default value is 0 (album mode disabled).
         *
         * This key is only used during decoding.
         */

        const val AAC_DRC_ALBUM_MODE: String = "aac-drc-album-mode"

        /**
         * A key describing the FLAC compression level to be used (FLAC audio format only).
         * The associated value is an integer ranging from 0 (fastest, least compression)
         * to 8 (slowest, most compression).
         */

        const val FLAC_COMPRESSION_LEVEL: String = "flac-compression-level"

        /**
         * A key describing the MPEG-H stream profile-level indication.
         *
         * See ISO_IEC_23008-3;2019 MHADecoderConfigurationRecord mpegh3daProfileLevelIndication.
         */

        const val MPEGH_PROFILE_LEVEL_INDICATION: String = "mpegh-profile-level-indication"

        /**
         * A key describing the MPEG-H stream compatible sets.
         *
         * See FDAmd_2 of ISO_IEC_23008-3;2019 MHAProfileAndLevelCompatibilitySetBox.
         */

        const val MPEGH_COMPATIBLE_SETS: String = "mpegh-compatible-sets"

        /**
         * A key describing the MPEG-H stream reference channel layout.
         *
         * See ISO_IEC_23008-3;2019 MHADecoderConfigurationRecord referenceChannelLayout
         * and ISO_IEC_23001‐8 ChannelConfiguration value.
         */

        const val MPEGH_REFERENCE_CHANNEL_LAYOUT: String = "mpegh-reference-channel-layout"

        /**
         * A key describing the encoding complexity.
         * The associated value is an integer.  These values are device and codec specific,
         * but lower values generally result in faster and/or less power-hungry encoding.
         *
         * @see MediaCodecInfo.EncoderCapabilities.getComplexityRange
         */

        const val COMPLEXITY: String = "complexity"

        /**
         * A key describing the desired encoding quality.
         * The associated value is an integer.  This key is only supported for encoders
         * that are configured in constant-quality mode.  These values are device and
         * codec specific, but lower values generally result in more efficient
         * (smaller-sized) encoding.
         *
         * @see MediaCodecInfo.EncoderCapabilities.getQualityRange
         */

        const val QUALITY: String = "quality"

        /**
         * A key describing the desired codec priority.
         *
         *
         * The associated value is an integer. Higher value means lower priority.
         *
         *
         * Currently, only two levels are supported:<br></br>
         * 0: realtime priority - meaning that the codec shall support the given
         * performance configuration (e.g. framerate) at realtime. This should
         * only be used by media playback, capture, and possibly by realtime
         * communication scenarios if best effort performance is not suitable.<br></br>
         * 1: non-realtime priority (best effort).
         *
         *
         * This is a hint used at codec configuration and resource planning - to understand
         * the realtime requirements of the application; however, due to the nature of
         * media components, performance is not guaranteed.
         *
         */

        const val PRIORITY: String = "priority"

        /**
         * A key describing the desired operating frame rate for video or sample rate for audio
         * that the codec will need to operate at.
         *
         *
         * The associated value is an integer or a float representing frames-per-second or
         * samples-per-second
         *
         *
         * This is used for cases like high-speed/slow-motion video capture, where the video encoder
         * format contains the target playback rate (e.g. 30fps), but the component must be able to
         * handle the high operating capture rate (e.g. 240fps).
         *
         *
         * This rate will be used by codec for resource planning and setting the operating points.
         *
         */

        const val OPERATING_RATE: String = "operating-rate"

        /**
         * A key describing the desired profile to be used by an encoder.
         *
         *
         * The associated value is an integer.
         * Constants are declared in [MediaCodecInfo.CodecProfileLevel].
         * This key is used as a hint, and is only supported for codecs
         * that specify a profile. When configuring profile, encoder configuration
         * may fail if other parameters are not compatible with the desired
         * profile or if the desired profile is not supported, but it may also
         * fail silently (where the encoder ends up using a different, compatible profile.)
         *
         *
         * **Note:** Codecs are free to use all the available
         * coding tools at the specified profile, but may ultimately choose to not do so.
         *
         *
         * **Note:** When configuring video encoders, profile must be
         * set together with [level][.LEVEL].
         *
         * @see MediaCodecInfo.CodecCapabilities.profileLevels
         */

        const val PROFILE: String = "profile"

        /**
         * A key describing the desired profile to be used by an encoder.
         *
         *
         * The associated value is an integer.
         * Constants are declared in [MediaCodecInfo.CodecProfileLevel].
         * This key is used as a further hint when specifying a desired profile,
         * and is only supported for codecs that specify a level.
         *
         *
         * This key is ignored if the [profile][.PROFILE] is not specified.
         * Otherwise, the value should be a level compatible with the configured encoding
         * parameters.
         *
         *
         * **Note:** This key cannot be used to constrain the encoder's
         * output to a maximum encoding level. Encoders are free to target a different
         * level if the configured encoding parameters dictate it. Nevertheless,
         * encoders shall use (and encode) a level sufficient to decode the generated
         * bitstream, though they may exceed the (Video) Buffering Verifier limits for
         * that encoded level.
         *
         * @see MediaCodecInfo.CodecCapabilities.profileLevels
         */

        const val LEVEL: String = "level"

        /**
         * An optional key describing the desired encoder latency in frames. This is an optional
         * parameter that applies only to video encoders. If encoder supports it, it should ouput
         * at least one output frame after being queued the specified number of frames. This key
         * is ignored if the video encoder does not support the latency feature. Use the output
         * format to verify that this feature was enabled and the actual value used by the encoder.
         *
         *
         * If the key is not specified, the default latency will be implenmentation specific.
         * The associated value is an integer.
         */

        const val LATENCY: String = "latency"

        /**
         * An optional key describing the maximum number of non-display-order coded frames.
         * This is an optional parameter that applies only to video encoders. Application should
         * check the value for this key in the output format to see if codec will produce
         * non-display-order coded frames. If encoder supports it, the output frames' order will be
         * different from the display order and each frame's display order could be retrived from
         * [MediaCodec.BufferInfo.presentationTimeUs]. Before API level 27, application may
         * receive non-display-order coded frames even though the application did not request it.
         * Note: Application should not rearrange the frames to display order before feeding them
         * to [MediaMuxer.writeSampleData].
         *
         *
         * The default value is 0.
         */

        const val OUTPUT_REORDER_DEPTH: String = "output-reorder-depth"

        /**
         * A key describing the desired clockwise rotation on an output surface.
         * This key is only used when the codec is configured using an output surface.
         * The associated value is an integer, representing degrees. Supported values
         * are 0, 90, 180 or 270. This is an optional field; if not specified, rotation
         * defaults to 0.
         *
         * @see MediaCodecInfo.CodecCapabilities.profileLevels
         */

        const val ROTATION: String = "rotation-degrees"

        /**
         * A key describing the desired bitrate mode to be used by an encoder.
         * Constants are declared in [MediaCodecInfo.EncoderCapabilities].
         *
         * @see MediaCodecInfo.EncoderCapabilities.isBitrateModeSupported
         */

        const val BITRATE_MODE: String = "bitrate-mode"

        /**
         * A key describing the maximum Quantization Parameter allowed for encoding video.
         * This key applies to all three video picture types (I, P, and B).
         * The value is used directly for picture type I; a per-mime formula is used
         * to calculate the value for the remaining picture types.
         *
         * This calculation can be avoided by directly specifying values for each picture type
         * using the type-specific keys [.VIDEO_QP_I_MAX], [.VIDEO_QP_P_MAX],
         * and [.VIDEO_QP_B_MAX].
         *
         * The associated value is an integer.
         */

        const val VIDEO_QP_MAX: String = "video-qp-max"

        /**
         * A key describing the minimum Quantization Parameter allowed for encoding video.
         * This key applies to all three video frame types (I, P, and B).
         * The value is used directly for picture type I; a per-mime formula is used
         * to calculate the value for the remaining picture types.
         *
         * This calculation can be avoided by directly specifying values for each picture type
         * using the type-specific keys [.VIDEO_QP_I_MIN], [.VIDEO_QP_P_MIN],
         * and [.VIDEO_QP_B_MIN].
         *
         * The associated value is an integer.
         */

        const val VIDEO_QP_MIN: String = "video-qp-min"

        /**
         * A key describing the maximum Quantization Parameter allowed for encoding video.
         * This value applies to video I-frames.
         *
         * The associated value is an integer.
         */

        const val VIDEO_QP_I_MAX: String = "video-qp-i-max"

        /**
         * A key describing the minimum Quantization Parameter allowed for encoding video.
         * This value applies to video I-frames.
         *
         * The associated value is an integer.
         */

        const val VIDEO_QP_I_MIN: String = "video-qp-i-min"

        /**
         * A key describing the maximum Quantization Parameter allowed for encoding video.
         * This value applies to video P-frames.
         *
         * The associated value is an integer.
         */

        const val VIDEO_QP_P_MAX: String = "video-qp-p-max"

        /**
         * A key describing the minimum Quantization Parameter allowed for encoding video.
         * This value applies to video P-frames.
         *
         * The associated value is an integer.
         */

        const val VIDEO_QP_P_MIN: String = "video-qp-p-min"

        /**
         * A key describing the maximum Quantization Parameter allowed for encoding video.
         * This value applies to video B-frames.
         *
         * The associated value is an integer.
         */

        const val VIDEO_QP_B_MAX: String = "video-qp-b-max"

        /**
         * A key describing the minimum Quantization Parameter allowed for encoding video.
         * This value applies to video B-frames.
         *
         * The associated value is an integer.
         */

        const val VIDEO_QP_B_MIN: String = "video-qp-b-min"

        /**
         * A key describing the level of encoding statistics information emitted from video encoder.
         *
         * The associated value is an integer.
         */

        const val VIDEO_ENCODING_STATISTICS_LEVEL: String = "video-encoding-statistics-level"
    }


    private val mMap: MutableMap<String, Any> = mutableMapOf()

    fun getString(key: String): String? {
        return mMap[key] as? String
    }

    fun setString(key: String, value: String) {
        mMap[key] = value
    }

    fun getInt(key: String, defaultValue: Int = 0): Int {
        if (mMap[key] == null) {
            return defaultValue
        }
        return mMap[key] as Int
    }

    fun setInt(key: String, value: Int) {
        mMap[key] = value
    }

    fun getFloat(key: String): Float? {
        return mMap[key] as? Float
    }

    fun setFloat(key: String, value: Float) {
        mMap[key] = value
    }

    fun getLong(key: String): Long? {
        return mMap[key] as? Long
    }

    fun setLong(key: String, value: Long) {
        mMap[key] = value
    }

    fun getKeys(): Array<String> {
        return mMap.keys.toTypedArray()
    }


    companion object {

        fun createVideoFormat(mime: String, width: Int, height: Int): MediaFormat1 {
            val format = MediaFormat1()
            format.setString(KEYS.MIME, mime)
            format.setInt(KEYS.WIDTH, width)
            format.setInt(KEYS.HEIGHT, height)
            return format
        }
    }




}