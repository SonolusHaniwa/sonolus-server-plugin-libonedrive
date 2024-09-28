auto OneDriveData = [](client_conn conn, http_request request, param argv){
    Json::Value driveConfig; string json = readFile("./plugins/libonedrive/config.json");
    json_decode(json, driveConfig);

    // 下载文件
    const string drive_api = "https://graph.microsoft.com/v1.0/me/drive/root:";
    const string auth_api = "https://login.microsoftonline.com/common/oauth2/v2.0/token";

    // 获取新token
    json = curl_post(auth_api, {},
    "client_id=" + driveConfig["onedrive.client.id"].asString() +
    "&redirect_uri=http://localhost" + 
    "&client_secret=" + driveConfig["onedrive.client.secret"].asString() + 
    "&refresh_token=" + driveConfig["onedrive.refresh.token"].asString() + "&grant_type=refresh_token");
    Json::Value res; json_decode(json, res);
    string token = res["access_token"].asString();

    // 获取文件
    json = curl_post(drive_api + "/data/" + argv[0], {{"Authorization", "Bearer " + token}, {"Content-Type", "application/json"}}, "{}");

    auto response = __api_default_response;
    response["Location"] = json_decode(json)["@microsoft.graph.downloadUrl"].asString();
    response["Content-Length"] = "0";
    putRequest(conn, 307, response);
};