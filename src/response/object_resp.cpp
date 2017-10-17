// Copyright (c) 2017, Tencent Inc.
// All rights reserved.
//
// Author: sevenyou <sevenyou@tencent.com>
// Created: 07/21/17
// Description:

#include "response/object_resp.h"

#include "rapidxml/1.13/rapidxml.hpp"
#include "rapidxml/1.13/rapidxml_print.hpp"
#include "rapidxml/1.13/rapidxml_utils.hpp"

#include "cos_params.h"
#include "cos_sys_config.h"
#include "util/string_util.h"

namespace qcloud_cos {

bool InitMultiUploadResp::ParseFromXmlString(const std::string& body) {
    rapidxml::xml_document<> doc;
    if (!StringUtil::StringToXml(body, &doc)) {
        SDK_LOG_ERR("Parse string to xml doc error, xml_body=%s", body.c_str());
        return false;
    }

    rapidxml::xml_node<>* root = doc.first_node(kInitiateMultipartUploadRoot.c_str());
    if (NULL == root) {
        SDK_LOG_ERR("Miss root node=InitiateMultipartUploadResult, xml_body=%s", body.c_str());
        return false;
    }

    rapidxml::xml_node<>* node = root->first_node();
    for (; node != NULL; node = node->next_sibling()) {
        const std::string& node_name = node->name();
        if (node_name == kInitiateMultipartUploadBucket) {
            m_bucket = node->value();
        } else if (node_name == kInitiateMultipartUploadKey) {
            m_key = node->value();
        } else if (node_name == kInitiateMultipartUploadId) {
            m_upload_id = node->value();
        }
    }

    return true;
}

bool CompleteMultiUploadResp::ParseFromXmlString(const std::string& body) {
    rapidxml::xml_document<> doc;
    if (!StringUtil::StringToXml(body, &doc)) {
        SDK_LOG_ERR("Parse string to xml doc error, xml_body=%s", body.c_str());
        return false;
    }

    rapidxml::xml_node<>* root = doc.first_node(kCompleteMultiUploadRoot.c_str());
    if (NULL == root) {
        SDK_LOG_ERR("Miss root node=ListBucketsResult, xml_body=%s", body.c_str());
        return false;
    }

    rapidxml::xml_node<>* node = root->first_node();
    for (; node != NULL; node = node->next_sibling()) {
        const std::string& node_name = node->name();
        if (node_name == kCompleteMultiUploadLocation) {
            m_location = node->value();
        } else if (node_name == kCompleteMultiUploadBucket) {
            m_bucket = node->value();
        } else if (node_name == kCompleteMultiUploadKey) {
            m_key = node->value();
        } else if (node_name == kCompleteMultiUploadETag) {
            SetEtag(node->value());
        }
    }

    return true;
}

void GetObjectResp::ParseFromHeaders(const std::map<std::string, std::string>& headers) {
    BaseResp::ParseFromHeaders(headers);
    std::map<std::string, std::string>::const_iterator itr;
    itr = headers.find(kRespHeaderLastModified);
    if (headers.end() != itr) {
        m_last_modified = itr->second;
    }

    itr = headers.find(kRespHeaderXCosObjectType);
    if (headers.end() != itr) {
        m_x_cos_object_type = itr->second;
    }

    itr = headers.find(kRespHeaderXCosStorageClass);
    if (headers.end() != itr) {
        m_x_cos_storage_class = itr->second;
    }
}

void HeadObjectResp::ParseFromHeaders(const std::map<std::string, std::string>& headers) {
    BaseResp::ParseFromHeaders(headers);
    std::map<std::string, std::string>::const_iterator itr;
    for (itr = headers.begin(); itr != headers.end(); ++itr) {
        const std::string& key = itr->first;
        if (key == kRespHeaderLastModified) {
            m_last_modified = itr->second;
        } else if (key == kRespHeaderXCosObjectType) {
            m_x_cos_object_type = itr->second;
        } else if (key == kRespHeaderXCosStorageClass) {
            m_x_cos_storage_class = itr->second;
        } else if (StringUtil::StringStartsWith(key, kXCosMetaPrefix)) {
            std::string remove_prefix = StringUtil::StringRemovePrefix(key, kXCosMetaPrefix);
            m_x_cos_metas[remove_prefix] = itr->second;
        }
    }
}

void MultiUploadObjectResp::CopyFrom(const InitMultiUploadResp& resp) {
    m_resp_tag = "Init";
    InternalCopyFrom(resp);
    m_upload_id = resp.GetUploadId();
    m_bucket = resp.GetBucket();
    m_key = resp.GetKey();
}

// TODO(sevenyou)
void MultiUploadObjectResp::CopyFrom(const UploadPartDataResp& resp) {
    m_resp_tag = "Upload";
    InternalCopyFrom(resp);
}

void MultiUploadObjectResp::CopyFrom(const CompleteMultiUploadResp& resp) {
    m_resp_tag = "Complete";
    InternalCopyFrom(resp);
    m_location = resp.GetLocation();
    m_bucket = resp.GetBucket();
    m_key = resp.GetKey();
}

void MultiUploadObjectResp::InternalCopyFrom(const BaseResp& resp) {
    SetContentLength(resp.GetContentLength());
    SetContentType(resp.GetContentType());
    SetEtag(resp.GetEtag());
    SetConnection(resp.GetConnection());
    SetDate(resp.GetDate());
    SetServer(resp.GetServer());
    SetXCosRequestId(resp.GetXCosRequestId());
    SetXCosTraceId(resp.GetXCosTraceId());

    SetHeaders(resp.GetHeaders());
    SetBody(resp.GetBody());
}

bool ListPartsResp::ParseFromXmlString(const std::string& body) {
    rapidxml::xml_document<> doc;
    if (!StringUtil::StringToXml(body, &doc)) {
        SDK_LOG_ERR("Parse string to xml doc error, xml_body=%s", body.c_str());
        return false;
    }

    rapidxml::xml_node<>* root = doc.first_node("ListPartsResult");
    if (NULL == root) {
        SDK_LOG_ERR("Miss root node=ListPartsResult, xml_body=%s", body.c_str());
        return false;
    }

    rapidxml::xml_node<>* node = root->first_node();
    for (; node != NULL; node = node->next_sibling()) {
        const std::string& node_name = node->name();
        if ("Bucket" == node_name) {
            m_bucket = node->value();
        } else if ("Encoding-type" == node_name) {
            m_encoding_type = node->value();
        } else if ("Key" == node_name) {
            m_key = node->value();
        } else if ("UploadId" == node_name) {
            m_upload_id = node->value();
        } else if ("Initiator" == node_name) {
            rapidxml::xml_node<>* init_node = node->first_node();
            for (; init_node != NULL; init_node = init_node->next_sibling()) {
                const std::string& init_node_name = init_node->name();
                if ("ID" == init_node_name) {
                    m_initiator.m_id = init_node->value();
                } else if ("DisplyName" == init_node_name) {
                    m_initiator.m_display_name = init_node->value();
                } else {
                    SDK_LOG_WARN("Unknown field in Initiator node, field_name=%s",
                                 init_node_name.c_str());
                }
            }
        } else if ("Owner" == node_name) {
            rapidxml::xml_node<>* owner_node = node->first_node();
            for (; owner_node != NULL; owner_node = owner_node->next_sibling()) {
                const std::string& owner_node_name = owner_node->name();
                if ("ID" == owner_node_name) {
                    m_owner.m_id = owner_node->value();
                } else if ("DisplyName" == owner_node_name) {
                    m_owner.m_display_name = owner_node->value();
                } else {
                    SDK_LOG_WARN("Unknown field in Owner node, field_name=%s",
                                 owner_node_name.c_str());
                }
            }
        } else if ("PartNumberMarker" == node_name) {
            m_part_number_marker = StringUtil::StringToUint64(node->value());
        } else if ("Part" == node_name) {
            rapidxml::xml_node<>* part_node = node->first_node();
            Part part;
            for (; part_node != NULL; part_node = part_node->next_sibling()) {
                const std::string& part_node_name = part_node->name();
                if ("PartNumber" == part_node_name) {
                    part.m_part_num = StringUtil::StringToUint64(part_node->value());
                } else if ("LastModified" == part_node_name) {
                    part.m_last_modified = part_node->value();
                } else if ("ETag" == part_node_name) {
                    part.m_etag = part_node->value();
                } else if ("Size" == part_node_name) {
                    part.m_size = StringUtil::StringToUint64(part_node->value());
                } else {
                    SDK_LOG_WARN("Unknown field in Part node, field_name=%s",
                                 part_node_name.c_str());
                }
            }
            m_parts.push_back(part);
        } else if ("NextPartNumberMarker" == node_name) {
            m_next_part_number_marker = StringUtil::StringToUint64(node->value());
        } else if ("StorageClass" == node_name) {
            m_storage_class = node->value();
        } else if ("MaxParts" == node_name) {
            m_max_parts = StringUtil::StringToUint64(node->value());
        } else if ("IsTruncated" == node_name) {
            m_is_truncated = (node->value() == "true") ? true : false;
        } else {
            SDK_LOG_WARN("Unknown field in ListPartsResult node, field_name=%s",
                         node_name.c_str());
        }
    }

    return true;
}

bool GetObjectACLResp::ParseFromXmlString(const std::string& body) {
    return ParseFromACLXMLString(body, &m_owner_id, &m_owner_display_name, &m_acl);
}

bool PutObjectCopyResp::ParseFromXmlString(const std::string& body) {
    rapidxml::xml_document<> doc;
    if (!StringUtil::StringToXml(body, &doc)) {
        SDK_LOG_ERR("Parse string to xml doc error, xml_body=%s", body.c_str());
        return false;
    }

    rapidxml::xml_node<>* root = doc.first_node("CopyObjectResult");
    if (NULL == root) {
        SDK_LOG_ERR("Miss root node=CopyObjectResult, xml_body=%s", body.c_str());
        return false;
    }

    rapidxml::xml_node<>* node = root->first_node();
    for (; node != NULL; node = node->next_sibling()) {
        const std::string& node_name = node->name();
        if ("ETag" == node_name) {
            m_etag = node->value();
        } else if ("LastModified" == node_name) {
            m_last_modified = node->value();
        } else {
            SDK_LOG_WARN("Unknown field in CopyObjectResult node, field_name=%s",
                         node_name.c_str());
        }
    }
    return true;
}

} // namespace qcloud_cos
