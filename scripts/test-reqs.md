```bash
curl -v \
  -X POST http://127.0.0.1:5000/test \
  -H "User-Agent: CustomClient/1.0" \
  -H "Accept: application/json" \
  -H "Content-Type: application/json" \
  -H "X-Custom-Header: MyHeaderValue" \
  -d '{"username":"rahul","password":"secret","nested":{"key":"value"},"array":[1,2,3,4,5]}' \
  -H "Connection: close"
```
