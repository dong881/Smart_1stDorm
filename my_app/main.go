package main

import (
        "encoding/json"
        "fmt"
        "log"
        "net/http"
        "os/exec"
        "time"
)

type User struct {
        Name  string `json:"name"`
        Email string `json:"email"`
}

var Run []User

func main() {
        fmt.Printf("Start server\n")

        // ............
        http.HandleFunc("/run", handleRun)

        fmt.Printf("Start Handle\n")

        // ......server
        log.Fatal(http.ListenAndServe(":8080", nil))
}
func handleRun(w http.ResponseWriter, r *http.Request) {
        // ......request method
        if r.Method == http.MethodGet {
                getRun(w, r)
                return
        }
        // ......request method......GET...POST.....................
        w.WriteHeader(http.StatusMethodNotAllowed)
        w.Write([]byte("Invalid request method"))
}
func getRun(w http.ResponseWriter, r *http.Request) {
        currentTime := time.Now()
        fmt.Println("GET request!!! at" , currentTime)
        re := "OK!"
        cmd := exec.Command("python", "./sw.py")
        err := cmd.Run()
        if err != nil {
                re += err.Error()
                log.Fatal(err)
        }
        // .....................
        json.NewEncoder(w).Encode(re)
}