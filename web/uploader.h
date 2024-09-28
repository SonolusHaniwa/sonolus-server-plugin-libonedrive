auto OneDriveUploader = [](client_conn conn, http_request request, param argv){
    if (!checkLogin(request)) {
        putRequest(conn, 401, __default_response);
        send(conn, json_encode(msg[401]));
        close(conn.conn);
        return;
    }
    auto $_POST = postParam(request);
    string filePointerBeg = base64_decode($_POST["file"]);

    Json::Value res2;
    res2["status"] = "success";
    res2["hash"] = onedrive_uploader(filePointerBeg);
    res2["url"] = "/data/" + res2["hash"].asString();
    __api_default_response["Content-Length"] = json_encode(res2).size();
    putRequest(conn, 200, __api_default_response);
    send(conn, json_encode(res2));
};