#!/bin/bash

# Test script for new abilities: Headshot, Exchange, Ambush

echo "========================================"
echo "  Testing New Abilities"
echo "========================================"
echo

PASSED=0
FAILED=0

# Function to run a test
run_test() {
    local test_name=$1
    local ability1=$2
    local ability2=$3
    local test_file=$4
    
    echo -n "Testing $test_name... "
    
    if ./raiinet -text -ability1 "$ability1" -ability2 "$ability2" < "$test_file" > /tmp/test_output.txt 2>&1; then
        if grep -q "Ability used successfully\|Player.*wins" /tmp/test_output.txt || ! grep -q "Ability failed" /tmp/test_output.txt; then
            echo "PASS"
            ((PASSED++))
        else
            echo "FAIL (ability failed unexpectedly)"
            ((FAILED++))
        fi
    else
        echo "FAIL (program error)"
        ((FAILED++))
    fi
}

# Headshot tests
run_test "Headshot - Basic" "HLDSP" "LFDSP" "tests/test_headshot_basic.txt"
run_test "Headshot - Attacker Stronger" "HLDSP" "LFDSP" "tests/test_headshot_attacker_stronger.txt"
run_test "Headshot - Attacker Weaker" "HLDSP" "LFDSP" "tests/test_headshot_attacker_weaker.txt"
run_test "Headshot - Data Download" "HLDSP" "LFDSP" "tests/test_headshot_data_download.txt"

# Exchange tests
run_test "Exchange - Basic" "ELDSP" "LFDSP" "tests/test_exchange_basic.txt"
run_test "Exchange - Middle Board" "ELDSP" "LFDSP" "tests/test_exchange_middle_board.txt"

# Ambush tests
run_test "Ambush - Basic" "ALDSP" "LFDSP" "tests/test_ambush_basic.txt"

# Combined test
run_test "All Three Abilities" "HELAD" "LFDSP" "tests/test_all_three_abilities.txt"

echo
echo "========================================"
echo "Results: $PASSED passed, $FAILED failed"
echo "========================================"

if [ $FAILED -eq 0 ]; then
    echo "All tests passed!"
    exit 0
else
    echo "Some tests failed."
    exit 1
fi

