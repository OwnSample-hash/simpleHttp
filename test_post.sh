#!/bin/bash

curl -v -H "Content-Type: application/json" -d '{"name": "John Doe", "email": ""}' http://localhost:8000/lom
