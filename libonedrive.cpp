#include"../../main.cpp"
#include<bits/stdc++.h>
#include<curl/curl.h>
using namespace std;

size_t req_reply(void *ptr, size_t size, size_t nmemb, void *stream) {
	string *str = (string*)stream;
	(*str).append((char*)ptr, size*nmemb);
	return size * nmemb;
}
string curl_post(string url, vector<pair<string, string> > header, string data) {
    CURL *curl = curl_easy_init();
    CURLcode res; string response;
    if (curl) {
        struct curl_slist* header_list = NULL;
        for (int i = 0; i < header.size(); i++) header_list = curl_slist_append(header_list, (header[i].first + ": " + header[i].second).c_str());
		curl_easy_setopt(curl, CURLOPT_HTTPHEADER, header_list);
		curl_easy_setopt(curl, CURLOPT_HEADER, 0);
		curl_easy_setopt(curl, CURLOPT_POST, 1);
		curl_easy_setopt(curl, CURLOPT_URL, url.c_str());
		curl_easy_setopt(curl, CURLOPT_POSTFIELDS, data.c_str());
		curl_easy_setopt(curl, CURLOPT_SSL_VERIFYPEER, false);
		curl_easy_setopt(curl, CURLOPT_SSL_VERIFYHOST, false);
		curl_easy_setopt(curl, CURLOPT_VERBOSE, 0);
		curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, req_reply);
		curl_easy_setopt(curl, CURLOPT_WRITEDATA, (void *)&response);
        res = curl_easy_perform(curl);
        curl_easy_cleanup(curl);
    }
    return response;
}

// curl put
typedef struct
{ 
    void *data; 
    int body_size; 
    int body_pos; 
} postdata; 
size_t readfunc(void *ptr, size_t size, size_t nmemb, void *stream)
{ 
    if (stream)
    { 
        postdata *ud = (postdata*) stream; 
        int available = (ud->body_size - ud->body_pos);
        if (available > 0)
        { 
            int written = min(size * nmemb, (size_t)available);
            memcpy(ptr, ((char*)(ud->data)) + ud->body_pos, written); 
            ud->body_pos += written;
            return written; 
        } 
    }
    return 0; 
} 
string curl_put(string url, vector<pair<string, string> > header, string data) {
    CURL *curl = curl_easy_init();
    CURLcode res; string response;
    postdata pd; pd.body_size = data.size();
    pd.body_pos = 0; pd.data = const_cast<char*>(data.c_str());
    if (curl) {
        struct curl_slist* header_list = NULL;
        for (int i = 0; i < header.size(); i++) header_list = curl_slist_append(header_list, (header[i].first + ": " + header[i].second).c_str());
        curl_easy_setopt(curl, CURLOPT_HTTPHEADER, header_list);
        curl_easy_setopt(curl, CURLOPT_HEADER, 0);
        curl_easy_setopt(curl, CURLOPT_PUT, 1);
        curl_easy_setopt(curl, CURLOPT_URL, url.c_str());
        curl_easy_setopt(curl, CURLOPT_SSL_VERIFYPEER, false);
        curl_easy_setopt(curl, CURLOPT_SSL_VERIFYHOST, false);
        curl_easy_setopt(curl, CURLOPT_VERBOSE, 0);
        curl_easy_setopt(curl, CURLOPT_READFUNCTION, readfunc);
        curl_easy_setopt(curl, CURLOPT_READDATA, (void *)&pd);
        res = curl_easy_perform(curl);
        curl_easy_cleanup(curl);
    }
    return response;
}

class PluginOneDrive: public SonolusServerPlugin {
    public:
    
    string onPluginName() const {
        return "OneDrive FileSystem Plugin";
    }
    string onPluginDescription() const {
        return "Provide Cloud File Storage Service";
    }
    string onPluginVersion() const {
        return "1.0.0";
    }
    string onPluginPlatformVersion() const {
        return sonolus_server_version;
    }
    string onPluginAuthor() const {
        return "LittleYang0531";
    }
    string onPluginLicense() const {
        return "MIT";
    }
    string onPluginWebsite() const {
        return "https://github.com/SonolusHaniwa/sonolus-server-plugin-libonedrive";
    }
    vector<string> onPluginHelp(char** argv) const {
        return {};
    }
    void onPluginRunner(int argc, char** argv) const {

    }
    void onPluginRouter(int argc, char** argv, application *app) const {
        preload();
        for (int i = 0; i < app->route.size(); i++) {
            if (app->route[i].path == "/uploader") {
                app->route[i].main = [](client_conn conn, http_request request, param argv){
                    if (!checkLogin(request)) {
                        putRequest(conn, 401, __default_response);
                        send(conn, json_encode(msg[401]));
                        close(conn.conn);
                        return;
                    }
                    auto $_POST = postParam(request); int len;
                    string filePointerBeg = base64_decode($_POST["file"]);
                    unsigned char* fileSha1 = sha1(const_cast<char*>(filePointerBeg.c_str()), filePointerBeg.size());
                    stringstream buffer;
                    for (int i = 0; i < 20; i++)
                        buffer << hex << setw(2) << setfill('0') << int(fileSha1[i]);

                    Json::Value driveConfig; string json = readFile("./plugins/libonedrive/config.json");
                    json_decode(json, driveConfig);

                    // 上传大文件
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

                    // 获取上传会话
                    json = curl_post(
                        drive_api + "/data/" + buffer.str() + ":/createUploadSession", 
                        {{"Authorization", "Bearer " + token},
                        {"Content-Type", "application/json"}}, 
                        "{\"item\": {\"@microsoft.graph.conflictBehavior\": \"rename\", \"name\": \"" + buffer.str() + "\"}}");
                    json_decode(json, res);
                    string url = res["uploadUrl"].asString();

                    // 上传文件
                    int size = 320 * 1024;
                    for (int ist = 0; ist < len; ist += size) {
                        int ed = min(ist + size, len);
                        json = curl_put(url, {
                            {"Authorization", "Bearer " + token}, 
                            {"Content-Length", to_string(ed - ist)}, 
                            {"Content-Range", "Bytes " + to_string(ist) + "-" + to_string(ed - 1) + "/" + to_string(len)}}, 
                            filePointerBeg.substr(ist, ed - ist));
                    };

                    Json::Value res2;
                    res2["status"] = "success";
                    res2["hash"] = buffer.str();
                    res2["url"] = "/data/" + buffer.str();
                    putRequest(conn, 200, __api_default_response);
                    send(conn, json_encode(res2));
                    close(conn.conn);
                };
            }
            if (app->route[i].path == "/data/%s") {
                app->route[i].main = [](client_conn conn, http_request request, param argv){
                    Json::Value driveConfig; string json = readFile("./plugins/libonedrive/config.json");
                    json_decode(json, driveConfig);

                    auto response = __api_default_response;
                    response["Location"] = driveConfig["oneindex.url"].asString() + "/data/" + argv[0];
                    putRequest(conn, 307, response);
                    close(conn.conn);
                };
            }
        }
    }
};

PLUMA_INHERIT_PROVIDER(PluginOneDrive, SonolusServerPlugin);

PLUMA_CONNECTOR
bool pluma_connect(pluma::Host& host) {
    host.add( new PluginOneDriveProvider() );
    return true;
}
