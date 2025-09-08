# Multi-Client Server

This project implements a multi-client server in C++ that can handle multiple client requests concurrently. Clients send jobs to the server, and the server processes them and responds with results. Supported job types include text processing, encoding/decoding, and compression tasks.

---

## **Compilation**

To compile the server and client, run:

```bash
make
```

This will generate the executables in the `output/` directory:

* `output/server` — the server executable
* `output/client` — the client executable

---

## **Server Usage**

Run the server with configurable options:

```bash
./output/server --queue <queue_type> --capacity <queue_capacity> --worker <num_workers> --port <port_number>
```

### **Server Options**

| Option       | Description                                                         | Default  |
| ------------ | ------------------------------------------------------------------- | -------- |
| `--queue`    | Type of request queue (`static`, `circular`, `dynamic` or `Bound Blocking`)                       | `static` |
| `--capacity` | Maximum number of requests in the queue (ignored for dynamic queue) | `10`     |
| `--worker`   | Number of worker threads to process requests                        | `8`      |
| `--port`     | TCP port for the server to listen on                                | `8080`   |

Example:

```bash
./output/server --queue dynamic --worker 12 --port 9090
```

This will start a server with a dynamic queue, 12 workers, listening on port 9090.

---

## **Client Usage**

Run the client executable:

```bash
./output/client
```

When started, the client will prompt you for the following information:

1. **Job Type**: Choose one of the supported job types:

   ```
   count-words
   extract-emails
   csv-to-tsv
   compress
   decompress
   base64-encode
   base64-decode
   ```

2. **Filename**: Enter the name of the file you want to process. This is used by the server to identify or save the request (optional for some jobs).
For example:
```
notes.txt →  text to count words in  eg: Hello world! This is a test file.

emails.txt → test extract-emails     eg: Hello alice@example.com, please contact bob@company.org

data.csv →  test csv-to-tsv          eg: Name,Age,Email

sample.txt → for compress/decompress or base64-encode/decode 
```

3. **Message/Payload**: Enter the content of the file or data you want the server to process.

The client sends this information as a JSON object to the server. The server processes the job and returns a JSON response containing:

* `id` — request ID
* `status` — `ok` or `error`
* `result` — the result of the job or an error message

---

### **Example Client Session**

```
Connected to server!
Enter job type (count-words / extract-emails / csv-to-tsv / compress / decompress / base64-encode / base64-decode or 'exit'): count-words
Enter filename: example.txt
Enter your message: Hello world! Welcome to multi-client server testing.

Server response: {"id": 1, "status": "ok", "result": "6 words"}
```

---

## **Running Multiple Clients Concurrently**

You can run multiple clients in parallel and log their outputs:

```bash
mkdir -p logs
for f in tests/files/*.txt; do
    ./output/client --file "$f" --job count-words >> logs/client.log 2>&1 &
done
wait
```

This will process all `.txt` files in `tests/files` using the `count-words` job, storing logs in `logs/client.log`.

---

## **Notes**

* The client supports interactive input or can accept command-line arguments for automation.
* The server supports both static and dynamic request queues.
* Make sure the server is running before starting clients.
* The client logs can be used to measure request latency or verify responses.

