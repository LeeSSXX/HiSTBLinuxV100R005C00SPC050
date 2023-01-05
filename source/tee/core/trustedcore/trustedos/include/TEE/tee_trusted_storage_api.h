/**
 * @file tee_trusted_storage_api.h
 *
 * Copyright(C), 2008-2013, Huawei Tech. Co., Ltd. ALL RIGHTS RESERVED. \n
 *
 * ��������ȫ���簲ȫ�洢���API  \n
*/

/**@defgroup TEE_TRUSTED_STORAGE_API  ��ȫ�洢
* @ingroup TEE_API
*/

#ifndef __TEE_TRUSTED_STORAGE_API_H
#define __TEE_TRUSTED_STORAGE_API_H

#include "tee_internal_api.h"

/**
 * @ingroup TEE_TRUSTED_STORAGE_API
 * HANDLE_NULL�Ķ��壬��Чobject handle
 */
#define TEE_HANDLE_NULL 0x00000000

typedef struct s_list_node {
	struct s_list_node *pstPrev;
	struct s_list_node *pstNext;
} s_list;

struct __TEE_ObjectEnumHandle {
	//TEE_UUID uuid;
	uint32_t storageID;
	char ObjectID[64];
	uint32_t objectEnumType;
	s_list listhead;
};
typedef struct  __TEE_ObjectEnumHandle *TEE_ObjectEnumHandle;

/**
 * @ingroup  TEE_TRUSTED_STORAGE_API
 *
 * ��������λ��ʼλ��ѡ�����#TEE_SeekObjectData���� \n
*/
typedef enum {
	TEE_DATA_SEEK_SET = 0,     /**< ��λ��ʼλ��Ϊ��������ʼ�� */
	TEE_DATA_SEEK_CUR,         /**< ��λ��ʼλ��Ϊ��ǰ������λ�� */
	TEE_DATA_SEEK_END          /**< ��λ��ʼλ��������ĩβ�� */
} TEE_Whence;

/**
 * @ingroup  TEE_TRUSTED_STORAGE_API
 *
 *  Storage ID�������Ӧapp�Ĵ洢�ռ� \n
*/
enum Object_Storage_Constants {
	TEE_OBJECT_STORAGE_PRIVATE = 0x00000001,     /**< ��Ӧÿ��Ӧ�õ�����˽�еĴ洢�ռ䣬Ŀǰ��֧����һ�� */
};

/**
 * @ingroup  TEE_TRUSTED_STORAGE_API
 *
 * #TEE_ObjectHandle��#handleFlags��������#TEE_ObjectHandle��object�������ķ���Ȩ�� \n
*/
enum Data_Flag_Constants {
	TEE_DATA_FLAG_ACCESS_READ = 0x00000001,        /**< ���������ж�Ȩ�ޣ����Խ��ж�����#TEE_ReadObjectData */
	TEE_DATA_FLAG_ACCESS_WRITE = 0x00000002,       /**< ����������дȨ�ޣ����Խ���д����#TEE_WriteObjectData�Ͳü�����#TEE_TruncateObjectData */
	TEE_DATA_FLAG_ACCESS_WRITE_META = 0x00000004,  /**< ����������WRITE_METAȨ�ޣ����Խ���ɾ��#TEE_CloseAndDeletePersistentObject�͸�������#TEE_RenamePersistentObject */
	TEE_DATA_FLAG_SHARE_READ = 0x00000010,         /**< ���������й����Ȩ�ޣ����Դ򿪶��#TEE_ObjectHandle������ȡ */
	TEE_DATA_FLAG_SHARE_WRITE = 0x00000020,        /**< ���������й���дȨ�ޣ����Դ򿪶��#TEE_ObjectHandle����д�� */
	TEE_DATA_FLAG_CREATE = 0x00000200,             /**< δʹ�� */
	TEE_DATA_FLAG_EXCLUSIVE = 0x00000400,          /**< ��������ͬ�������ļ������ͬ���ļ������ڣ����½������ļ������ͬ���ļ������򱨴� */
	TEE_DATA_FLAG_OVERWRITE = 0x00000400,          /**< ��������ͬ�������ļ������ͬ���ļ������ڣ����½������ļ������ͬ���ļ������򱨴� */
	TEE_DATA_FLAG_AES256 = 	0x10000000,		/**< bit28���Ϊ1����AES256, Ϊ0����AES128 */
	TEE_DATA_FLAG_OPEN_AESC = 0x20000000,		/**< bit29���Ϊ1�������Ȱ��յͰ汾��ʽ�� */
};

/**
 * @ingroup  TEE_TRUSTED_STORAGE_API
 *
 * #TEE_ObjectHandle �� #keyUsage��������object key���÷� \n
*/
enum Usage_Constants {
	TEE_USAGE_EXTRACTABLE = 0x00000001,    /**< object��key������ȡ */
	TEE_USAGE_ENCRYPT = 0x00000002,        /**< object��key�������ڼ��� */
	TEE_USAGE_DECRYPT = 0x00000004,        /**< object��key�������ڽ��� */
	TEE_USAGE_MAC = 0x00000008,            /**< object��key����hash���� */
	TEE_USAGE_SIGN = 0x00000010,           /**< object��key����ǩ������ */
	TEE_USAGE_VERIFY = 0x00000020,         /**< object��key������֤���� */
	TEE_USAGE_DERIVE = 0x00000040,         /**< object��key���ڲ���key�Ĳ��� */
	TEE_USAGE_DEFAULT = 0xFFFFFFFF,        /**< object��ʼ��ʹ�ã�Ĭ�ϸ�������Ȩ�� */
};

/**
 * @ingroup  TEE_TRUSTED_STORAGE_API
 *
 * #TEE_ObjectHandle ��#handleFlags��������object��һЩ��Ϣ���Ƿ�������object���Ƿ��ʼ���ȵ� \n
*/
enum Handle_Flag_Constants {
	TEE_HANDLE_FLAG_PERSISTENT = 0x00010000,        /**< objectΪ����object */
	TEE_HANDLE_FLAG_INITIALIZED = 0x00020000,       /**< object�Ѿ���ʼ�� */
	TEE_HANDLE_FLAG_KEY_SET = 0x00040000,           /**< δʹ�� */
	TEE_HANDLE_FLAG_EXPECT_TWO_KEYS = 0x00080000,   /**< δʹ�� */
};

/**
 * @ingroup  TEE_TRUSTED_STORAGE_API
 *
 * ϵͳ��Դ���ƣ���������λ��ָʾ����ȡ���ֵ  \n
*/
enum Miscellaneous_Constants {
	TEE_DATA_MAX_POSITION = 0xFFFFFFFF,    /**< ��������λ��ָʾ����ȡ������ֽڳ��� */
	TEE_OBJECT_ID_MAX_LEN = 64,            /**< objectID������ֽڳ��� */
};

/**
 * @ingroup  TEE_TRUSTED_STORAGE_API
 *
 * ���������Դ洢���ݵ�����ֽ��� \n
*/
enum TEE_DATA_Size {
	TEE_DATA_OBJECT_MAX_SIZE = 0xFFFFFFFF     /**< object���������Դ洢���ݵ�����ֽ��� */
};

/**
 * @ingroup  TEE_TRUSTED_STORAGE_API
 * @brief ��ȡobject��Ϣ
 *
 * @par ����:
 * ��ȡobject��#TEE_ObjectInfo�����俽��������objectInfoָ��Ŀռ䣬�ÿռ����û�Ԥ����
 *
 * @attention ��
 * @param object [IN]  ��ȡ#TEE_ObjectInfo��Դ#TEE_ObjectHandle
 * @param objectInfo [OUT]  ָ��ṹ���ָ�룬�ýṹ��������ŵõ���#TEE_ObjectInfo
 *
 * @retval ��
 *
 * @par ����:
 * @li Tee_trusted_storage_api.h���ýӿ��������ڵ�ͷ�ļ���
 * @since TrustedCore V100R002C00B302
*/

void TEE_GetObjectInfo(TEE_ObjectHandle object, TEE_ObjectInfo *objectInfo);

/**
 * @ingroup  TEE_TRUSTED_STORAGE_API
 * @brief ����object��#objectUsageλ
 *
 * @par ����:
 * ����object��#objectUsageλ����λ����object��key���÷���ȡֵ��ΧΪ#Usage_Constants�����ڲ���objectUsage��flagλ��˵: \n
 *   �����λ����Ϊ1���򲻻�ı�object��usage flag  \n
 *   �����λ����Ϊ0����object��Ӧ��object usage flag����  \n
 * ����ԭ����flagλ�������
 *
 * @attention �½���object���������#Usage_Constants����usage flagֻ�ܱ����㣬���ܱ���λ
 * @param object [IN]  ��Ҫ���Ƶ�#TEE_ObjectHandle
 * @param objectUsage [IN]  �û���ı��#objectUsage
 *
 * @retval ��
 *
 * @par ����:
 * @li Tee_trusted_storage_api.h���ýӿ��������ڵ�ͷ�ļ���
 * @since TrustedCore V100R002C00B302
*/

void TEE_RestrictObjectUsage(TEE_ObjectHandle object, uint32_t objectUsage);

/**
 * @ingroup  TEE_TRUSTED_STORAGE_API
 * @brief ��ȡobject��attribute
 *
 * @par ����:
 * ��ȡ#TEE_ObjectHandle��ָ��object��#TEE_Attribute�ṹ����union��buffer���ݣ��Ҹ�union��Ա����Ϊref
 *
 * @attention #TEE_Attribute�ṹ����union�ĳ�Ա����Ϊref�����������#TEE_AttributeΪ˽�ܵ���Ҫ��object��#Usage_Constants
 * �������#TEE_USAGE_EXTRACTABLE
 * @param object [IN]  ��ȡ#TEE_Attribute��Դ#TEE_ObjectHandle
 * @param attributeID [IN]  ��Ҫ��ȡ#TEE_Attribute��ID������#TEE_ObjectAttribute��Ҳ���Զ���
 * @param buffer [OUT]  ָ�룬ָ���buffer������Ż�ȡ����buffer�е�����
 * @param size [OUT]  ָ�룬��������ֽڳ���
 *
 * @retval #TEE_SUCCESS ��ʾ�ú���ִ�гɹ�
 * @retval #TEE_ERROR_ITEM_NOT_FOUND ��object��û�з���Ҫ�ҵ�#TEE_Attribute�����߸�objectδ��ʼ��
 * @retval #TEE_ERROR_SHORT_BUFFER �ṩ��buffer̫С��������ȫ��Ż�ȡ��������
 *
 * @par ����:
 * @li Tee_trusted_storage_api.h���ýӿ��������ڵ�ͷ�ļ���
 * @since TrustedCore V100R002C00B302
*/

TEE_Result TEE_GetObjectBufferAttribute(TEE_ObjectHandle object, uint32_t attributeID, void *buffer, size_t *size);

/**
 * @ingroup  TEE_TRUSTED_STORAGE_API
 * @brief ��ȡobject��attribute
 *
 * @par ����:
 * ��ȡ#TEE_ObjectHandle��ָ��object��#TEE_Attribute�ṹ����union��value���Ҹ�union��Ա����Ϊvalue
 *
 * @attention #TEE_Attribute�ṹ����union�ĳ�Ա����Ϊvalue�����������#TEE_AttributeΪ˽�ܵ���Ҫ��object��#Usage_Constants
 * �������#TEE_USAGE_EXTRACTABLE
 * @param object [IN]  ��ȡ#TEE_Attribute��Դ#TEE_ObjectHandle
 * @param attributeID [IN]  ��Ҫ��ȡ#TEE_Attribute��ID������#TEE_ObjectAttribute��Ҳ���Զ���
 * @param a [OUT]  ָ�룬ָ��Ŀռ��������value�ṹ���a
 * @param b [OUT]  ָ�룬ָ��Ŀռ��������value�ṹ���b
 *
 * @retval #TEE_SUCCESS ��ʾ�ú���ִ�гɹ�
 * @retval #TEE_ERROR_ITEM_NOT_FOUND ��object��û�з���Ҫ�ҵ�#TEE_Attribute�����߸�objectδ��ʼ��
 * @retval #TEE_ERROR_ACCESS_DENIED ��ͼ��ȡһ��˽�ܵ�#TEE_Attribute��û����λ#TEE_USAGE_EXTRACTABLE
 *
 * @par ����:
 * @li Tee_trusted_storage_api.h���ýӿ��������ڵ�ͷ�ļ���
 * @since TrustedCore V100R002C00B302
*/

TEE_Result TEE_GetObjectValueAttribute(TEE_ObjectHandle object, uint32_t attributeID, uint32_t *a, uint32_t *b);

/**
 * @ingroup  TEE_TRUSTED_STORAGE_API
 * @brief �رմ򿪵�#TEE_ObjectHandle
 *
 * @par ����:
 * �رմ򿪵�#TEE_ObjectHandle����object����������object������ʱobject
 *
 * @attention ��
 * @param object [IN]  ��Ҫ�رյ�#TEE_ObjectHandle
 *
 * @retval ��
 *
 * @par ����:
 * @li Tee_trusted_storage_api.h���ýӿ��������ڵ�ͷ�ļ���
 * @since TrustedCore V100R002C00B302
*/

void TEE_CloseObject(TEE_ObjectHandle object);

/**
 * @ingroup  TEE_TRUSTED_STORAGE_API
 * @brief ����һ��δ��ʼ����object
 *
 * @par ����:
 * ����һ��δ��ʼ����object���������key������#objectType��#maxObjectSize����ָ������Ԥ������Դ
 *
 * @attention ��
 * @param objectType [IN]  ��������object�����ͣ�ȡֵ����Ϊ#TEE_ObjectType
 * @param maxObjectSize [IN]  object������ֽ���
 * @param object [OUT]  ָ�룬ָ���´�����object��handle
 *
 * @retval #TEE_SUCCESS ��ʾ�ú���ִ�гɹ�
 * @retval #TEE_ERROR_OUT_OF_MEMORY û���㹻����Դȥ����
 * @retval #TEE_ERROR_NOT_SUPPORTED ��object�ṩ���ֽ�������֧��
 *
 * @par ����:
 * @li Tee_trusted_storage_api.h���ýӿ��������ڵ�ͷ�ļ���
 * @since TrustedCore V100R002C00B302
*/
TEE_Result TEE_AllocateTransientObject(uint32_t objectType, uint32_t maxObjectSize, TEE_ObjectHandle *object);

/**
 * @ingroup  TEE_TRUSTED_STORAGE_API
 * @brief �ͷ�һ���ѷ�����Դ����ʱobject
 *
 * @par ����:
 * �ͷ�һ���ѷ�����Դ����ʱobject���������ú��handleʧЧ�����ͷ����з������Դ����#TEE_AllocateTransientObject���ʹ��
 *
 * @attention ��
 * @param object [IN]  ��Ҫ�ͷŵ�#TEE_ObjectHandle
 *
 * @retval ��
 *
 * @par ����:
 * @li Tee_trusted_storage_api.h���ýӿ��������ڵ�ͷ�ļ���
 * @since TrustedCore V100R002C00B302
*/

void TEE_FreeTransientObject(TEE_ObjectHandle object);

/**
 * @ingroup  TEE_TRUSTED_STORAGE_API
 * @brief ����transient object
 *
 * @par ����:
 * ������ʱobject�����ĳ�ʼ״̬������allocate֮���״̬:�ѷ�����Դ��û�д���key��δ��ʼ��object������������������key
 *
 * @attention ��
 * @param object [IN]  ��Ҫ���õ�#TEE_ObjectHandle
 *
 * @retval ��
 *
 * @par ����:
 * @li Tee_trusted_storage_api.h���ýӿ��������ڵ�ͷ�ļ���
 * @since TrustedCore V100R002C00B302
*/

void TEE_ResetTransientObject(TEE_ObjectHandle object);

/**
 * @ingroup  TEE_TRUSTED_STORAGE_API
 * @brief ��һ��δ��ʼ������ʱobject��attribute
 *
 * @par ����:
 * �ú���������attrs�е�attributes����һ��δ��ʼ������ʱobject������attrs�ɿ���Ӧ��(Trusted APP)�ṩ
 *
 * @attention ��
 * @param object [IN/OUT]  �Ѵ�����δ��ʼ����#TEE_ObjectHandle
 * @param attrs [IN]  object attribute���飬����Ϊһ�����߶��#TEE_Attribute
 * @param attrCount [IN]  �����Ա����
 *
 * @retval #TEE_SUCCESS ��ʾ�ú���ִ�гɹ�
 * @retval #TEE_ERROR_BAD_PARAMETERS attribute����ȷ���߲�һ�£���ʱ���뱣֤object��Ȼδ��ʼ��
 *
 * @par ����:
 * @li Tee_trusted_storage_api.h���ýӿ��������ڵ�ͷ�ļ���
 * @since TrustedCore V100R002C00B302
*/
TEE_Result TEE_PopulateTransientObject(TEE_ObjectHandle object, TEE_Attribute *attrs, uint32_t attrCount);

/**
 * @ingroup  TEE_TRUSTED_STORAGE_API
 * @brief ��ʼ��һ��buffer���͵�#TEE_Attribute
 *
 * @par ����:
 * ��ʼ��һ��buffer���͵�#TEE_Attribute����#TEE_Attribute�ṹ����union�ĳ�Ա����Ϊref
 *
 * @attention ��
 * @param attr [OUT]  ��Ҫ��ʼ����#TEE_Attribute
 * @param attributeID [IN]  ����#TEE_Attribute��ID
 * @param buffer [IN]  ��buffer���Ҫ��ֵ������
 * @param length [IN]  ��ֵ���ݵ��ֽڳ���
 *
 * @retval ��
 *
 * @par ����:
 * @li Tee_trusted_storage_api.h���ýӿ��������ڵ�ͷ�ļ���
 * @since TrustedCore V100R002C00B302
*/
void TEE_InitRefAttribute(TEE_Attribute *attr, uint32_t attributeID, void *buffer, size_t length);

/**
 * @ingroup  TEE_TRUSTED_STORAGE_API
 * @brief ��ʼ��һ��value���͵�#TEE_Attribute����#TEE_Attribute�ṹ����union�ĳ�Ա����Ϊvalue
 *
 * @par ����:
 * ��ʼ��һ��value���͵�#TEE_Attribute
 *
 * @attention ��
 * @param attr [OUT]  ��Ҫ��ʼ����#TEE_Attribute
 * @param attributeID [IN]  ����#TEE_Attribute ��ID
 * @param a [IN]  ��ֵ��#TEE_Attribute�ṹ����union�ĳ�Աvalue b��ֵ
 * @param b [IN]  ��ֵ��#TEE_Attribute�ṹ����union�ĳ�Աvalue a��ֵ
 *
 * @retval ��
 *
 * @par ����:
 * @li Tee_trusted_storage_api.h���ýӿ��������ڵ�ͷ�ļ���
 * @since TrustedCore V100R002C00B302
*/

void TEE_InitValueAttribute(TEE_Attribute *attr, uint32_t attributeID, uint32_t a, uint32_t b);

/**
 * @ingroup  TEE_TRUSTED_STORAGE_API
 * @brief ��һ����ʼ����object��δ��ʼ����object��ֵ#TEE_Attribute
 *
 * @par ����:
 * �ú�����һ����ʼ����object��һ��δ��ʼ����object��ֵ#TEE_Attribute���൱�ڰ�srcobject��#TEE_Attribute������destobject��
 *
 * @attention ����object��#TEE_Attribute ���͡���������ƥ��
 * @param destObject [IN]  δ��ʼ����Ҫ��ֵ��#TEE_ObjectHandle
 * @param srcObject [IN]  �ѳ�ʼ����������һ��object��ֵ��#TEE_ObjectHandle
 *
 * @retval ��
 *
 * @par ����:
 * @li Tee_trusted_storage_api.h���ýӿ��������ڵ�ͷ�ļ���
 * @since TrustedCore V100R002C00B302
*/

void TEE_CopyObjectAttributes(TEE_ObjectHandle destObject, TEE_ObjectHandle srcObject);

/**
 * @ingroup  TEE_TRUSTED_STORAGE_API
 * @brief �������key����key-pair
 *
 * @par ����:
 * �ú����������key����key-pair������ֵ����ʱobject
 *
 * @attention ��
 * @param object [IN]  ��ʱobject��������Ų�����key
 * @param keySize [IN]  Ҫ���key���ֽڴ�С
 * @param params [IN]  ����key��Ҫ�Ĳ���
 * @param paramCount [IN]  ����key��Ҫ�Ĳ�������
 *
 * @retval #TEE_SUCCESS ��ʾ�ú���ִ�гɹ�
 * @retval #TEE_ERROR_BAD_PARAMETERS: #������key����ʱobject���ܴ�ŵ�key���Ͳ�һ��
 *
 * @par ����:
 * @li Tee_trusted_storage_api.h���ýӿ��������ڵ�ͷ�ļ���
 * @since TrustedCore V100R002C00B302
*/
TEE_Result TEE_GenerateKey(TEE_ObjectHandle object, uint32_t keySize, TEE_Attribute *params, uint32_t paramCount);

/**
 * @ingroup  TEE_TRUSTED_STORAGE_API
 * @brief ����һ���µ�����object
 *
 * @par ����:
 * ����һ���µ�����object������ֱ�ӳ�ʼ����������#TEE_Attribute���û������÷��ص�handle������object��#TEE_Attribute��������
 *
 * @attention ��
 * @param storageID [IN]   ��Ӧÿ��Ӧ�õ����Ĵ洢�ռ䣬ȡֵΪ#Object_Storage_Constants��Ŀǰ��֧��#TEE_STORAGE_PRIVATE.
 * @param objectID [IN]  object  identifier����Ҫ������object������
 * @param objectIDLen [IN]  object  identifier ���ֽڳ���
 * @param flags [IN]  object�������flags��ȡֵ����Ϊ#Data_Flag_Constants �� #Handle_Flag_Constants�е�һ������
 * @param attributes [IN]  ��ʱobject��#TEE_ObjectHandle��������ʼ��object��#TEE_Attribute������Ϊ#TEE_HANDLE_NULL
 * @param initialData [IN]  ��ʼ���ݣ�������ʼ������������
 * @param initialDataLen [IN] initialData ��ʼ���ݵ��ֽڳ���
 * @param object [OUT]  �����ɹ�ִ�к󷵻ص�#TEE_ObjectHandle
 *
 * @retval #TEE_SUCCESS ��ʾ�ú���ִ�гɹ�
 * @retval #TEE_ERROR_ITEM_NOT_FOUND:   ��storageID �����ڻ��߲����ҵ�object identifier
 * @retval #TEE_ERROR_ACCESS_CONFLICT ����Ȩ�޳�ͻ
 * @retval #TEE_ERROR_OUT_OF_MEMORY û���㹻����Դ����ɲ���
 * @retval #TEE_ERROR_STORAGE_NO_SPACE û���㹻�Ŀռ�������object
 *
 * @par ����:
 * @li Tee_trusted_storage_api.h���ýӿ��������ڵ�ͷ�ļ���
 * @since TrustedCore V100R002C00B302
*/
TEE_Result TEE_CreatePersistentObject(uint32_t storageID, void *ojbectID, size_t objectIDLen, uint32_t flags, TEE_ObjectHandle attributes, void *initialData, size_t initialDataLen, TEE_ObjectHandle *object);

/**
 * @ingroup  TEE_TRUSTED_STORAGE_API
 * @brief ��һ���Ѵ��ڵ�����object
 *
 * @par ����:
 * ��һ���Ѵ��ڵ�����object�����ص�handle�û�������������object��#TEE_Attribute��������
 *
 * @attention ��
 * @param storageID [IN]   ��Ӧÿ��Ӧ�õ����Ĵ洢�ռ䣬ȡֵΪ#Object_Storage_Constants��Ŀǰ��֧��#TEE_STORAGE_PRIVATE
 * @param objectID [IN]  object  identifier����Ҫ�򿪵�object������
 * @param objectIDLen [IN]  object  identifier ���ֽڳ���
 * @param flags [IN]  object�򿪺��flags��ȡֵ����Ϊ#Data_Flag_Constants �� #Handle_Flag_Constants�е�һ������
 * @param object [OUT]  �����ɹ�ִ�к󷵻ص�#TEE_ObjectHandle
 *
 * @retval #TEE_SUCCESS ��ʾ�ú���ִ�гɹ�
 * @retval #TEE_ERROR_ITEM_NOT_FOUND:   storageID �����ڻ��߲����ҵ�object identifier
 * @retval #TEE_ERROR_ACCESS_CONFLICT ����Ȩ�޳�ͻ
 * @retval #TEE_ERROR_OUT_OF_MEMORY û���㹻����Դ����ɲ���
 *
 * @par ����:
 * @li Tee_trusted_storage_api.h���ýӿ��������ڵ�ͷ�ļ���
 * @since TrustedCore V100R002C00B302
*/
TEE_Result TEE_OpenPersistentObject(uint32_t storageID, void *ojbectID, size_t objectIDLen, uint32_t flags, TEE_ObjectHandle *object);

/**
 * @ingroup  TEE_TRUSTED_STORAGE_API
 * @brief ��object����������ȡ���ݣ�object����Ϊ����object
 *
 * @par ����:
 * �ú�����object����������ȡsize�ֽ����ݵ�bufferָ��ָ����buffer,��#TEE_ObjectHandle�����Ѿ���#TEE_DATA_FLAG_ACCESS_READȨ�޴�
 *
 * @attention ��
 * @param objbect [IN]   Ҫ��ȡ��Դ#TEE_ObjectHandle
 * @param buffer [OUT]  ��Ŷ�ȡ����
 * @param size [IN]  Ҫ��ȡ�������ֽ���
 * @param count [OUT]  ʵ�ʶ�ȡ���������ֽ���
 *
 * @retval #TEE_SUCCESS ��ʾ�ú���ִ�гɹ���Ŀǰ�����سɹ����������󷵻�ֵ���ں����汾ʵ��
 *
 * @par ����:
 * @li Tee_trusted_storage_api.h���ýӿ��������ڵ�ͷ�ļ���
 * @since TrustedCore V100R002C00B302
*/
TEE_Result TEE_ReadObjectData(TEE_ObjectHandle ojbect, void *buffer, size_t size, uint32_t *count);

/**
 * @ingroup  TEE_TRUSTED_STORAGE_API
 * @brief ��object��������д�����ݣ�object����Ϊ����object
 *
 * @par ����:
 * �ú�����buffer��object��������д��size�ֽڵ�����,#TEE_ObjectHandle��������#TEE_DATA_FLAG_ACCESS_WRITEȨ�޴�
 *
 * @attention ��
 * @param ojbect [IN]   Ҫд���Ŀ��#TEE_ObjectHandle
 * @param buffer [IN]  ���Ҫд���Դ����
 * @param size [IN]  Ҫд��������ֽ���
 *
 * @retval #TEE_SUCCESS ��ʾ�ú���ִ�гɹ�
 *
 * @par ����:
 * @li Tee_trusted_storage_api.h���ýӿ��������ڵ�ͷ�ļ���
 * @since TrustedCore V100R002C00B302
*/
TEE_Result TEE_WriteObjectData(TEE_ObjectHandle ojbect, void *buffer, size_t size);

/**
 * @ingroup  TEE_TRUSTED_STORAGE_API
 * @brief �ı�object����������С��object����Ϊ����object
 *
 * @par ����:
 * �ú����ı��������ֽڴ�С�����sizeС�ڵ�ǰ��������size��ɾ�����г������ֽڡ����size���ڵ�ǰ��������size����'0'��չ
 * #TEE_ObjectHandle������#TEE_DATA_FLAG_ACCESS_WRITEȨ�޴�
 *
 * @attention ��
 * @param object [IN]   Ҫ�޸ĵ�#TEE_ObjectHandle
 * @param size [IN]  ���������µ����ݴ�С
 *
 * @retval #TEE_SUCCESS ��ʾ�ú���ִ�гɹ�
 * @retval #TEE_ERROR_STORAGE_NO_SPACE û���㹻�Ŀռ���ִ�в���
 *
 * @par ����:
 * @li Tee_trusted_storage_api.h���ýӿ��������ڵ�ͷ�ļ���
 * @since TrustedCore V100R002C00B302
*/

TEE_Result TEE_TruncateObjectData(TEE_ObjectHandle object, uint32_t size);

/**
 * @ingroup  TEE_TRUSTED_STORAGE_API
 * @brief ����object��������λ��ָʾ����object����Ϊ����object
 *
 * @par ����:
 * �ú�������#TEE_ObjectHandle��ָ���������λ�ã���������λ������Ϊ:ƫ�Ƶ���ʼλ��+offset \n
 * ����whence����ƫ�Ƶ���ʼλ�ã�ȡֵ����Ϊ#TEE_Whence���������£�\n
 *	   #TEE_DATA_SEEK_SET��������ƫ�Ƶ���ʼλ��Ϊ�ļ�ͷ����0��  \n
 *	   #TEE_DATA_SEEK_CUR��������ƫ�Ƶ���ʼλ��Ϊ��ǰλ�ã����ڵ�ǰλ�õĻ�����ƫ��offset  \n
 *	   #TEE_DATA_SEEK_END��������ƫ�Ƶ���ʼλ��Ϊ�ļ�ĩβ  \n
 * ����offsetΪ����ʱ���ƫ�ƣ������ļ�β�����򣬸���ʱ��ǰƫ�ƣ����ļ�ͷ������
 *
 * @attention ��
 * @param object [IN]   Ҫ���õ�#TEE_ObjectHandle
 * @param offset [IN]  ������λ���ƶ����ֽ���
 * @param whence [IN]  ����������ƫ�Ƶĳ�ʼλ��
 *
 * @retval #TEE_SUCCESS ��ʾ�ú���ִ�гɹ�
 * @retval #TEE_ERROR_OVERFLOW ����ʹλ��ָʾ����ֵ��������ϵͳ����#TEE_DATA_MAX_POSITION
 *
 * @par ����:
 * @li Tee_trusted_storage_api.h���ýӿ��������ڵ�ͷ�ļ���
 * @since TrustedCore V100R002C00B302
*/

TEE_Result TEE_SeekObjectData(TEE_ObjectHandle object, int32_t offset, TEE_Whence whence);

/**
 * @ingroup  TEE_TRUSTED_STORAGE_API
 * @brief �رմ򿪵�#TEE_ObjectHandle����ɾ��object
 *
 * @par ����:
 * �رմ򿪵�#TEE_ObjectHandle����ɾ��object��object����Ϊ����object�ұ�������#TEE_DATA_FLAG_ACCESS_WRITE_METAȨ�޴�
 *
 * @attention ��
 * @param object [IN]  ��Ҫ�رղ�ɾ����#TEE_ObjectHandle
 *
 * @retval ��
 *
 * @par ����:
 * @li Tee_trusted_storage_api.h���ýӿ��������ڵ�ͷ�ļ���
 * @since TrustedCore V100R002C00B302
 */

void TEE_CloseAndDeletePersistentObject(TEE_ObjectHandle object);


/**
 * @ingroup  TEE_TRUSTED_STORAGE_API
 * @brief ��ȡobject  ����Ϣ: ���ݲ��ֳ��Ⱥ��������ĵ�ǰλ��
 *
 * @par ����:
 * ��ȡobject���ݲ��ֵ���Ϣ�����ݲ��ֵ��ܳ��Ⱥ���������ǰ��λ��
 *
 * @attention ��
 *  *@param object [IN]   Ҫ���õ�#TEE_ObjectHandle
 * @param pos [out]  ������λ��
 * @param len [IN]  ����������
 *
 * @retval ��
 *
 * @par ����:
 * @li Tee_trusted_storage_api.h���ýӿ��������ڵ�ͷ�ļ���
 * @since TrustedCore V100R002C00B302
 */

TEE_Result TEE_InfoObjectData(TEE_ObjectHandle object, uint32_t *pos, uint32_t *len);
/**
 * @ingroup  TEE_TRUSTED_STORAGE_API
 * @brief ͬ���򿪵�#TEE_ObjectHandle������
 *
 * @par ����:
 * ͬ���򿪵�#TEE_ObjectHandle����ͬ����Ӧ�İ�ȫ�����ļ�(4��)������
 *
 * @attention ��
 * @param object [IN]  ��Ҫͬ����#TEE_ObjectHandle
 *
 * @retval #TEE_SUCCESS ��ʾ�ú���ִ�гɹ�
 *
 * @par ����:
 * @li Tee_trusted_storage_api.h���ýӿ��������ڵ�ͷ�ļ���
 * @since TrustedCore V100R002C00B302
 */

TEE_Result TEE_SyncPersistentObject(TEE_ObjectHandle object);

/**
 * @ingroup  TEE_TRUSTED_STORAGE_API
 * @brief �ı�object identifier�����ı�object������
 *
 * @par ����:
 * �ı�object identifier����#TEE_ObjectHandle������#TEE_DATA_FLAG_ACCESS_WRITE_METAȨ�޴�
 *
 * @attention ��
 * @param ojbect [IN/OUT]   Ҫ�޸ĵ�object handle
 * @param newObjectID [IN]  �µ�object identifier
 * @param newObjectIDLen [IN]  �µ�object identifier����
 *
 * @retval #TEE_SUCCESS ��ʾ�ú���ִ�гɹ�
 *
 * @par ����:
 * @li Tee_trusted_storage_api.h���ýӿ��������ڵ�ͷ�ļ���
 * @since TrustedCore V100R002C00B302
 */
TEE_Result TEE_RenamePersistentObject(TEE_ObjectHandle  object, void *newObjectID, size_t newObjectIDLen);

/**
 * @ingroup  TEE_TRUSTED_STORAGE_API
 * @brief ����һ��δ��ʼ����object enumerator ��handle
 *
 * @par ����:
 * ����һ��δ��ʼ����object enumerator��handle
 *
 * @attention ��
 * @param object [OUT]  ָ�룬ָ���´�����object enumerator �� handle
 *
 * @retval #TEE_SUCCESS ��ʾ�ú���ִ�гɹ�
 * @retval #TEE_ERROR_OUT_OF_MEMORY û���㹻����Դȥ����
 *
 * @par ����:
 * @li Tee_trusted_storage_api.h���ýӿ��������ڵ�ͷ�ļ���
 * @since TrustedCore V100R002C00B302
 */
TEE_Result TEE_AllocatePersistentObjectEnumerator(TEE_ObjectEnumHandle *objectEnumerator);

/**
 * @ingroup  TEE_TRUSTED_STORAGE_API
 * @brief �ͷ�һ���ѷ�����Դ��object enumerator handle
 *
 * @par ����:
 * �ͷ�һ���ѷ�����Դ����ʱobject enumerator handle���������ú��handleʧЧ�����ͷ����з������Դ����#TEE_AllocatePersistentObjectEnumerator���ʹ��
 *
 * @attention ��
 * @param object [IN]  ��Ҫ�ͷŵ�#TEE_ObjectEnumHandle
 *
 * @retval ��
 *
 * @par ����:
 * @li Tee_trusted_storage_api.h���ýӿ��������ڵ�ͷ�ļ���
 * @since TrustedCore V100R002C00B302
 */
void TEE_FreePersistentObjectEnumerator(TEE_ObjectEnumHandle objectEnumerator);

/**
 * @ingroup  TEE_TRUSTED_STORAGE_API
 * @brief ����object enumerator
 *
 * @par ����:
 * ������ʱobject enumerator �����ĳ�ʼ״̬������allocate֮���״̬
 *
 * @attention ��
 * @param object [IN]  ��Ҫ���õ�object enumerator ��#TEE_ObjectEnumHandle
 *
 * @retval ��
 *
 * @par ����:
 * @li Tee_trusted_storage_api.h���ýӿ��������ڵ�ͷ�ļ���
 * @since TrustedCore V100R002C00B302
 */
void TEE_ResetPersistentObjectEnumerator(TEE_ObjectEnumHandle objectEnumerator);

/**
 * @ingroup  TEE_TRUSTED_STORAGE_API
 * @brief ��ʼ��object enumerator����ʼ�оٸ����洢�ռ������object��object����Ϣ����ͨ��#TEE_GetNextPersistentObject������ȡ
 *
 * @par ����:
 * ��ʼ�оٸ����洢�ռ������object��object����Ϣ����ͨ��#TEE_GetNextPersistentObject������ȡ
 *
 * @attention ��
 * @param object [IN]  �ѷ���õ�object enumerator ��#TEE_ObjectEnumHandle
 * @param storageID [IN]   ��Ӧÿ��Ӧ�õ����Ĵ洢�ռ䣬ȡֵΪ#Object_Storage_Constants��Ŀǰ��֧��#TEE_STORAGE_PRIVATE
 *
 * @retval #TEE_SUCCESS ��ʾ�ú���ִ�гɹ�
 * @retval #TEE_ITEM_NOT_FOUND storageID����#TEE_STORAGE_PRIVATE������#TEE_STORAGE_PRIVATE�洢�ռ���û��object
 *
 * @par ����:
 * @li Tee_trusted_storage_api.h���ýӿ��������ڵ�ͷ�ļ���
 * @since TrustedCore V100R002C00B302
 */
TEE_Result TEE_StartPersistentObjectEnumerator(TEE_ObjectEnumHandle objectEnumerator,  uint32_t   storageID);

/**
 * @ingroup  TEE_TRUSTED_STORAGE_API
 * @brief ��ȡobject enumerator�е���һ��object��������object��#TEE_ObjectInfo, objectID , objectIDLen
 *
 * @par ����:
 *  ��ȡobject enumerator�е���һ��object��������object��#TEE_ObjectInfo, objectID , objectIDLen��Ϣ
 *
 * @attention ��
 * @param object [IN]  �ѳ�ʼ���õ�object enumerator ��#TEE_ObjectEnumHandle
 * @param objectInfo [OUT]  ָ��ṹ���ָ�룬�ýṹ��������ŵõ���#TEE_ObjectInfo
 * @param objectInfo [OUT]  ָ��һ��buffer��ָ�룬������ŵõ���objectID
 * @param objectInfo [OUT]  ������ŵõ���objectIDLen
 *
 * @retval #TEE_SUCCESS ��ʾ�ú���ִ�гɹ�
 * @retval #TEE_ITEM_NOT_FOUND enumerator�Ѿ�û��object����enumeratorû�б���ʼ��
 *
 * @par ����:
 * @li Tee_trusted_storage_api.h���ýӿ��������ڵ�ͷ�ļ���
 * @since TrustedCore V100R002C00B302
 */
TEE_Result TEE_GetNextPersistentObject(TEE_ObjectEnumHandle objectEnumerator,  TEE_ObjectInfo *objectInfo, void *objectID,  size_t *objectIDLen);

#endif
/**
 * History: \n
 * 2013-3-10��sdk����ʼ�����ļ� \n
 * 2013-6-27��sdk������Persistent Object Enumeration Functions  \n
 *
 * vi: set expandtab ts=4 sw=4 tw=80:
*/
