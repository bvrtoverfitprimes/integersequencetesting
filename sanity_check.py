import json
import math

def is_prime(n):
    if n < 2: return False
    for i in range(2, int(math.isqrt(n)) + 1):
        if n % i == 0: return False
    return True

with open("test_results.json") as f:
    data = json.load(f)["test_results"]

ok = True
for s in data["solutions"]:
    p, n = s["p"], s["n"]
    if not all(is_prime(p + d) for d in (0, 2, 6, 8)): ok = False; break
    if n != p * (p + 8): ok = False; break
    if n + 12 != (p + 2) * (p + 6): ok = False; break
    if (p + 1) * (p + 5) - (p - 1) * (p + 7) != 12: ok = False; break
    if (p + 3) * (p + 7) - (p + 1) * (p + 9) != 12: ok = False; break
    if n % 72 != 65: ok = False; break
    if is_prime(n): ok = False; break

print("Sanity check passed" if ok else "Sanity check failed")
