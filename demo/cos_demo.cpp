// Copyright (c) 2017, Tencent Inc.
// All rights reserved.
//
// Author: sevenyou <sevenyou@tencent.com>
// Created: 07/18/17
// Description:

#include <iostream>
#include <map>
#include <string>
#include <vector>

#include "util/auth_tool.h"
#include "cos_api.h"
#include "cos_sys_config.h"
#include "cos_defines.h"

using namespace qcloud_cos;
void PrintResult(const qcloud_cos::CosResult& result, const qcloud_cos::BaseResp& resp) {
    if (result.IsSucc()) {
        std::cout << resp.DebugString() << std::endl;
    } else {
        std::cout << "ErrorInfo=" << result.GetErrorInfo() << std::endl;
        std::cout << "HttpStatus=" << result.GetHttpStatus() << std::endl;
        std::cout << "ErrorCode=" << result.GetErrorCode() << std::endl;
        std::cout << "ErrorMsg=" << result.GetErrorMsg() << std::endl;
        std::cout << "ResourceAddr=" << result.GetResourceAddr() << std::endl;
        std::cout << "XCosRequestId=" << result.GetXCosRequestId() << std::endl;
        std::cout << "XCosTraceId=" << result.GetXCosTraceId() << std::endl;
    }
}


void PutObjectByFile(qcloud_cos::CosAPI& cos, const std::string& bucket_name,
                     const std::string& object_name, const std::string& file_path) {
    qcloud_cos::PutObjectByFileReq req(bucket_name, object_name, file_path);
    ImageProcessRule image_process_rule;
    image_process_rule.SetIsOriginalPic(true);
    image_process_rule.AddImageRule("test.png", "imageView2/format/png");
    image_process_rule.AddImageRule("test.bmp", "imageView2/format/bmp");
    req.SetImageProcessRule(image_process_rule);
    //req.AddParam("versionId", "MTg0NDY3NDI1NjQ4NjUyMjQ1MTA");
    qcloud_cos::PutObjectByFileResp resp;
    qcloud_cos::CosResult result = cos.PutObject(req, &resp);

    std::cout << "===================PutObjectResponse=====================" << std::endl;
    PrintResult(result, resp);
    ImagResp image_resp = resp.GetImageResp();
    std::cout << image_resp.DebugString();
    
    std::cout << "=========================================================" << std::endl;
}

void PutObjectByStream(qcloud_cos::CosAPI& cos, const std::string& bucket_name, const std::string& file_path) {
    std::ifstream ifs(file_path.c_str(), std::ios::in | std::ios::binary);
    if (!ifs.is_open()) {
        std::cout << ("Open local file fail, local file=" + file_path);
        return;
    }
    
    qcloud_cos::PutObjectByStreamReq req(bucket_name, "sevenyou_10m", ifs);
    ImageProcessRule image_process_rule;
    image_process_rule.SetIsOriginalPic(true);
    image_process_rule.AddImageRule("test.png", "imageView2/format/png");
    image_process_rule.AddImageRule("test.bmp", "imageView2/format/bmp");
    req.SetImageProcessRule(image_process_rule);
    
    qcloud_cos::PutObjectByStreamResp resp;
    qcloud_cos::CosResult result = cos.PutObject(req, &resp);

    std::cout << "===================PutObjectResponse=====================" << std::endl;
    PrintResult(result, resp);

    ImagResp image_resp = resp.GetImageResp();
    std::cout << image_resp.DebugString();
    std::cout << "=========================================================" << std::endl;
}

void InitMultiUpload(qcloud_cos::CosAPI& cos, const std::string& bucket_name,
                     const std::string& object_name, std::string* upload_id) {
    qcloud_cos::InitMultiUploadReq req(bucket_name, object_name);
    qcloud_cos::InitMultiUploadResp resp;
    qcloud_cos::CosResult result = cos.InitMultiUpload(req, &resp);

    std::cout << "=====================InitMultiUpload=====================";
    PrintResult(result, resp);
    std::cout << "=========================================================";

    *upload_id = resp.GetUploadId();
}

void UploadPartData(qcloud_cos::CosAPI& cos, const std::string& bucket_name,
                    const std::string& object_name, const std::string& upload_id,
                    std::fstream& is, uint64_t number,
                    std::string* etag) {
    qcloud_cos::UploadPartDataReq req(bucket_name, object_name,
                                      upload_id, is);
    req.SetPartNumber(number);
    qcloud_cos::UploadPartDataResp resp;
    qcloud_cos::CosResult result = cos.UploadPartData(req, &resp);
    *etag = resp.GetEtag();

    std::cout << "======================UploadPartData=====================";
    PrintResult(result, resp);
    std::cout << "=========================================================";
}

void AbortMultiUpload(qcloud_cos::CosAPI& cos, const std::string& bucket_name,
                      const std::string& object_name, const std::string& upload_id) {
    qcloud_cos::AbortMultiUploadReq req(bucket_name, object_name,
                                        upload_id);
    qcloud_cos::AbortMultiUploadResp resp;
    qcloud_cos::CosResult result = cos.AbortMultiUpload(req, &resp);
    std::cout << "======================AbortUploadPart=====================";
    PrintResult(result, resp);
    std::cout << "=========================================================";
}

void CompleteMultiUpload(qcloud_cos::CosAPI& cos, const std::string& bucket_name,
                         const std::string& object_name, const std::string& upload_id,
                         const std::vector<std::string>& etags,
                         const std::vector<uint64_t>& numbers) {
    qcloud_cos::CompleteMultiUploadReq req(bucket_name, object_name, upload_id);
    qcloud_cos::CompleteMultiUploadResp resp;
    req.AddParam("versionId", "MTg0NDY3NDI1NjQ4NjUyMjQ1MTA");
    req.SetEtags(etags);
    req.SetPartNumbers(numbers);
    ImageProcessRule image_process_rule;
    image_process_rule.SetIsOriginalPic(true);
    image_process_rule.AddImageRule("test.png", "imageView2/format/png");
    image_process_rule.AddImageRule("test.bmp", "imageView2/format/bmp");
    req.SetImageProcessRule(image_process_rule);

    qcloud_cos::CosResult result = cos.CompleteMultiUpload(req, &resp);

    std::cout << "===================Complete=============================" << std::endl;
    PrintResult(result, resp);
    ImagResp image_resp = resp.GetImageResp();
    std::cout << image_resp.DebugString();
    std::cout << "========================================================" << std::endl;
}

void MultiUploadObject(qcloud_cos::CosAPI& cos, const std::string& bucket_name,
                       const std::string& object_name, const std::string& local_file) {
    qcloud_cos::MultiUploadObjectReq req(bucket_name,
                                         object_name, local_file);
    ImageProcessRule image_process_rule;
    image_process_rule.SetIsOriginalPic(true);
    image_process_rule.AddImageRule("test.png", "imageView2/format/png");
    image_process_rule.AddImageRule("test.bmp", "imageView2/format/bmp");
    req.SetImageProcessRule(image_process_rule);

    //设置分块大小
    req.SetPartSize(1024 * 1024);
    
    req.SetRecvTimeoutInms(1000 * 60);
    qcloud_cos::MultiUploadObjectResp resp;
    qcloud_cos::CosResult result = cos.MultiUploadObject(req, &resp);

    if (result.IsSucc()) {
        std::cout << "MultiUpload Succ." << std::endl;
        std::cout << resp.GetLocation() << std::endl;
        std::cout << resp.GetKey() << std::endl;
//        std::cout << resp.GetBucket() << std::endl;
        std::cout << resp.GetEtag() << std::endl;
        ImagResp image_resp = resp.GetImageResp();
    std::cout << image_resp.DebugString();
    } else {
        std::cout << "MultiUpload Fail." << std::endl;
        // 获取具体失败在哪一步
        std::string resp_tag = resp.GetRespTag();
        if ("Init" == resp_tag) {
            // print result
        } else if ("Upload" == resp_tag) {
            // print result
        } else if ("Complete" == resp_tag) {
            // print result
        }
    }
    std::cout << "===================MultiUpload=============================" << std::endl;
    PrintResult(result, resp);
    std::cout << "========================================================" << std::endl;
}

void ListParts(qcloud_cos::CosAPI& cos, const std::string& bucket_name,
               const std::string& object_name, const std::string& upload_id) {
    qcloud_cos::ListPartsReq req(bucket_name, object_name, upload_id);
    req.SetMaxParts(1);
    req.SetPartNumberMarker("1");
    qcloud_cos::ListPartsResp resp;
    qcloud_cos::CosResult result = cos.ListParts(req, &resp);

    std::cout << "===================ListParts=====================" << std::endl;
    PrintResult(result, resp);
    std::cout << "====================================================================" << std::endl;
}



int main(int argc, char** argv) {
    qcloud_cos::CosConfig config("./config.json");
    qcloud_cos::CosAPI cos(config);

    std::string bucket_name = "tarketang-1251668577";


    //// 简单上传(文件)
    PutObjectByFile(cos, bucket_name, "sevenyou_1102_south", "./big_2.jpg");

    // 简单上传文件 stream方式
    PutObjectByStream(cos, bucket_name, "./big_2.jpg");

    //分块上传
    {
        std::string upload_id;
        std::string object_name = "sevenyou_e2_1102_north_multi";
        std::vector<uint64_t> numbers;
        std::vector<std::string> etags;

        std::string etag1 = "", etag2 = "";
        InitMultiUpload(cos, bucket_name, object_name, &upload_id);

        std::fstream is1("./big_2.jpg_1");
        UploadPartData(cos, bucket_name, object_name, upload_id, is1, 1, &etag1);
        numbers.push_back(1);
        etags.push_back(etag1);
        is1.close();

        ListParts(cos, bucket_name, object_name, upload_id);
        // AbortMultiUpload(cos, bucket_name, object_name, upload_id);

        std::fstream is2("./big_2.jpg_2");
        UploadPartData(cos, bucket_name, object_name, upload_id, is2, 2, &etag2);
        numbers.push_back(2);
        etags.push_back(etag2);

        is2.close();

        CompleteMultiUpload(cos, bucket_name, object_name, upload_id, etags, numbers);
    }

    //分块上传文件
    MultiUploadObject(cos, bucket_name, "sevenyou_1102_south_mulit", "./big_2.jpg");

    return 0;

}
