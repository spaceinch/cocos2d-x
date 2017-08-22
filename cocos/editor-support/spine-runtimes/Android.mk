LOCAL_PATH := $(call my-dir)
include $(CLEAR_VARS)

LOCAL_MODULE := spine_static

LOCAL_MODULE_FILENAME := libspine

ifeq ($(USE_ARM_MODE),1)
LOCAL_ARM_MODE := arm
endif

LOCAL_SRC_FILES := \
spine-c/src/spine/Animation.c \
spine-c/src/spine/AnimationState.c \
spine-c/src/spine/AnimationStateData.c \
spine-c/src/spine/Atlas.c \
spine-c/src/spine/AtlasAttachmentLoader.c \
spine-c/src/spine/Attachment.c \
spine-c/src/spine/AttachmentLoader.c \
spine-c/src/spine/Bone.c \
spine-c/src/spine/BoneData.c \
spine-c/src/spine/BoundingBoxAttachment.c \
spine-c/src/spine/Event.c \
spine-c/src/spine/EventData.c \
spine-c/src/spine/extension.c \
spine-c/src/spine/IkConstraint.c \
spine-c/src/spine/IkConstraintData.c \
spine-c/src/spine/Json.c \
spine-c/src/spine/MeshAttachment.c \
spine-c/src/spine/PathAttachment.c \
spine-c/src/spine/PathConstraint.c \
spine-c/src/spine/PathConstraintData.c \
spine-c/src/spine/RegionAttachment.c \
spine-c/src/spine/Skeleton.c \
spine-c/src/spine/SkeletonBounds.c \
spine-c/src/spine/SkeletonData.c \
spine-c/src/spine/SkeletonJson.c \
spine-c/src/spine/Skin.c \
spine-c/src/spine/Slot.c \
spine-c/src/spine/SlotData.c \
spine-c/src/spine/TransformConstraint.c \
spine-c/src/spine/TransformConstraintData.c \
spine-c/src/spine/VertexAttachment.c \
spine-cocos2dx/src/spine/AttachmentVertices.cpp \
spine-cocos2dx/src/spine/Cocos2dAttachmentLoader.cpp \
spine-cocos2dx/src/spine/SkeletonAnimation.cpp \
spine-cocos2dx/src/spine/SkeletonBatch.cpp \
spine-cocos2dx/src/spine/SkeletonRenderer.cpp \
spine-cocos2dx/src/spine/spine-cocos2dx.cpp

LOCAL_EXPORT_C_INCLUDES := $(LOCAL_PATH)/spine-cocos2dx/src \
$(LOCAL_PATH)/spine-c/include/

LOCAL_C_INCLUDES := $(LOCAL_PATH)/spine-cocos2dx/src \
$(LOCAL_PATH)/spine-c/include/

LOCAL_STATIC_LIBRARIES := cocos2dx_internal_static

include $(BUILD_STATIC_LIBRARY)
