# libonedrive

适用于 Sonolus Server for C++ 的 OneDrive 文件存储系统。

## 链接

- [Sonolus Website](https://sonolus.com/)
- [Sonolus Wiki](https://wiki.sonolus.com/)

## 编译指令

```cpp
g++ plugins/libonedrive/libonedrive.cpp -o plugins/libonedrive.so -fPIC -shared -std=c++17 -lcurl -DENABLE_MYSQL -DENABLE_SQLITE
```

## 配置文件

- `onedrive.api.token`: 获取 access_token 的 api 地址，具体见 [企业版账户登录](https://learn.microsoft.com/zh-cn/onedrive/developer/rest-api/getting-started/graph-oauth?view=odsp-graph-online) 和 [个人版账户登录](https://learn.microsoft.com/zh-cn/onedrive/developer/rest-api/getting-started/msa-oauth?view=odsp-graph-online)。
- `onedrive.client.id`: Azure Application Client ID。获取方法可见 [获取 client_id 和 client_secret](https://ovi.swo.moe/zh/docs/advanced)。
- `onedrive.client.secret`: Azure Application Client Secret。获取方法可见 [获取 client_id 和 client_secret](https://ovi.swo.moe/zh/docs/advanced)。
- `onedrive.refresh.token`: OneDrive Refresh Token。具体见 [企业版账户登录](https://learn.microsoft.com/zh-cn/onedrive/developer/rest-api/getting-started/graph-oauth?view=odsp-graph-online) 和 [个人版账户登录](https://learn.microsoft.com/zh-cn/onedrive/developer/rest-api/getting-started/msa-oauth?view=odsp-graph-online)。