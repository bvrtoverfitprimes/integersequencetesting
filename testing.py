from __future__ import annotations

import json
from typing import Dict, List, Tuple

LIMIT = 10**7

def build_spf(n: int) -> List[int]:
    """
    Linear sieve: returns an array spf of length n+1 such that
    spf[k] is the smallest prime factor of k, for 2 <= k <= n.
    spf[0] = spf[1] = 0 by convention.

    Runs in O(n) time and O(n) memory.
    """
    spf = [0] * (n + 1)
    primes: List[int] = []
    for i in range(2, n + 1):
        if spf[i] == 0:
            spf[i] = i
            primes.append(i)
        for p in primes:
            if p > spf[i] or i * p > n:
                break
            spf[i * p] = p
    return spf

def factorize(n: int, spf: List[int]) -> Dict[int, int]:
    """Return the prime factorization of n as a dict {prime: exponent}."""
    factors: Dict[int, int] = {}
    while n > 1:
        p = spf[n]
        factors[p] = factors.get(p, 0) + 1
        n //= p
    return factors

def phi_sigma(n: int, spf: List[int]) -> Tuple[int, int]:
    """
    Compute (phi(n), sigma(n)) using the prime factorization.

    For n = prod p_i^{e_i}:
        phi(n)   = prod p_i^{e_i - 1} * (p_i - 1)
        sigma(n) = prod (p_i^{e_i + 1} - 1) / (p_i - 1)
    """
    if n == 1:
        return 1, 1
    phi, sig = 1, 1
    for p, e in factorize(n, spf).items():
        phi *= (p - 1) * p ** (e - 1)
        sig *= (p ** (e + 1) - 1) // (p - 1)
    return phi, sig

def is_prime_quadruplet(p: int, spf: List[int]) -> bool:
    """A prime quadruplet is (p, p+2, p+6, p+8) with all four entries prime."""
    return all(spf[p + d] == p + d for d in (0, 2, 6, 8))

def main() -> None:
    spf = build_spf(LIMIT + 12)

    phis = [0] * (LIMIT + 13)
    sigs = [0] * (LIMIT + 13)
    for k in range(1, LIMIT + 13):
        phis[k], sigs[k] = phi_sigma(k, spf)

    solutions: List[int] = []
    for n in range(2, LIMIT):
        if spf[n] == n:
            continue
        if phis[n + 12] - phis[n] != 12:
            continue
        if sigs[n + 12] - sigs[n] != 12:
            continue
        solutions.append(n)

    print(f"Total solutions in [2, {LIMIT}):   {len(solutions)}")
    print(f"Distinct residues mod 72:        {sorted({n % 72 for n in solutions})}")
    print()
    print("Table 1 -- all composite n with both jump conditions:")
    print()
    print(f"{'p':>6} {'n = p(p+8)':>14} {'p+2':>6} {'p+6':>6} "
          f"{'n+12 = (p+2)(p+6)':>20} {'n mod 72':>10}")
    print("-" * 70)

    all_from_quadruplet = True
    all_residue_65 = True
    solution_details = []
    for n in solutions:
        fact_n = factorize(n, spf)
        primes_n = sorted(p for p, e in fact_n.items() for _ in range(e))
        if len(primes_n) != 2 or primes_n[1] - primes_n[0] != 8:
            print(f"  ANOMALY: n = {n} has factorization {fact_n}")
            all_from_quadruplet = False
            continue
        p, q = primes_n

        if not is_prime_quadruplet(p, spf):
            print(f"  ANOMALY: n = {n} = {p} * {q} but (p,p+2,p+6,p+8) "
                  f"is not a prime quadruplet")
            all_from_quadruplet = False
            continue

        assert n + 12 == (p + 2) * (p + 6), \
            f"Identity failed: {n} + 12 != ({p}+2)*({p}+6)"

        r = n % 72
        if r != 65:
            all_residue_65 = False
        print(f"{p:>6} {n:>14} {p+2:>6} {p+6:>6} {n+12:>20} {r:>10}")
        solution_details.append({
            "p": p,
            "n": n,
            "p_plus_2": p + 2,
            "p_plus_6": p + 6,
            "n_plus_12": n + 12,
            "n_mod_72": r
        })

    print()
    print(f"  All solutions arise from prime quadruplets: {all_from_quadruplet}")
    print(f"  All solutions are 65 (mod 72):              {all_residue_65}")
    print()
    if all_from_quadruplet and all_residue_65:
        print("CONSISTENT.")

    results = {
        "test_results": {
            "limit": int(LIMIT),
            "total_solutions": len(solutions),
            "distinct_residues_mod_72": sorted({n % 72 for n in solutions}),
            "solutions": solution_details,
            "verification": {
                "all_from_quadruplet": all_from_quadruplet,
                "all_residue_65": all_residue_65,
                "consistent_with_theorems": all_from_quadruplet and all_residue_65
            }
        }
    }

    with open("test_results.json", "w") as f:
        json.dump(results, f, indent=2)

if __name__ == "__main__":
    main()

