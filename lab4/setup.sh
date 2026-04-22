#!/bin/bash

# Colors for output
RED='\033[0;31m'
GREEN='\033[0;32m'
YELLOW='\033[1;33m'
NC='\033[0m' # No Color

# Get the directory where the script is located
SCRIPT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"

echo -e "${GREEN}=== Fitness Tracker API - Setup Script ===${NC}"
echo ""

# Check if Docker is installed
if ! command -v docker &> /dev/null; then
    echo -e "${RED}Docker is not installed!${NC}"
    echo "Please install Docker from https://www.docker.com/"
    exit 1
fi

# Check if Docker Compose is installed (plugin or legacy binary)
if docker compose version &> /dev/null; then
    COMPOSE_CMD="docker compose"
elif command -v docker-compose &> /dev/null; then
    COMPOSE_CMD="docker-compose"
else
    echo -e "${RED}Docker Compose is not installed!${NC}"
    echo "Please install Docker Compose"
    exit 1
fi

echo -e "${GREEN}✓ Docker and Docker Compose are installed${NC}"
echo ""

# Build and run the services
echo -e "${YELLOW}Building and starting services...${NC}"
cd "$SCRIPT_DIR"

$COMPOSE_CMD down 2>/dev/null

echo ""
echo -e "${YELLOW}Starting PostgreSQL and Fitness Tracker API...${NC}"
$COMPOSE_CMD up --build

# Check if services started successfully
sleep 5
if $COMPOSE_CMD ps | grep -qi "postgres.*up" && $COMPOSE_CMD ps | grep -qi "api.*up"; then
    echo ""
    echo -e "${GREEN}=== Services started successfully! ===${NC}"
    echo ""
    echo "API URL: http://localhost:8080"
    echo ""
    echo -e "${YELLOW}Example requests:${NC}"
    echo ""
    echo "1. Create user:"
    echo "   curl -X POST http://localhost:8080/users \\"
    echo "     -H 'Content-Type: application/json' \\"
    echo "     -d '{\"login\":\"john\",\"name\":\"John\",\"surname\":\"Doe\",\"email\":\"john@test.com\"}'"
    echo ""
    echo "2. Get user:"
    echo "   curl http://localhost:8080/users/john"
    echo ""
    echo "3. Create exercise:"
    echo "   curl -X POST http://localhost:8080/exercises \\"
    echo "     -H 'Content-Type: application/json' \\"
    echo "     -d '{\"name\":\"Bench Press\",\"description\":\"Chest\",\"muscle_group\":\"chest\"}'"
    echo ""
    echo "4. Create workout:"
    echo "   curl -X POST http://localhost:8080/workouts \\"
    echo "     -H 'Content-Type: application/json' \\"
    echo "     -d '{\"user_id\":1,\"date\":\"2026-03-27\"}'"
    echo ""
else
    echo -e "${RED}Failed to start services!${NC}"
    echo -e "${YELLOW}Checking logs:${NC}"
    $COMPOSE_CMD logs
    exit 1
fi
