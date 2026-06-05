# Integer Sequences: Conjecture Testing

Testing a conjecture about composite numbers with special properties related to Euler's totient function and the divisor sum function.

## The Conjecture

$$\{ \text{composite } n \mid \varphi(n+12) = \varphi(n) + 12 \land \sigma(n+12) = \sigma(n) + 12 \} \implies n \equiv 65 \pmod{72}$$

For any composite number $n$ satisfying **both**:
- $\varphi(n+12) = \varphi(n) + 12$
- $\sigma(n+12) = \sigma(n) + 12$

then $n \equiv 65 \pmod{72}$.

Every solution found takes the form $n = p(p+8)$ where $(p,\, p+2,\, p+6,\, p+8)$ is a prime quadruplet, which algebraically guarantees both conditions:

$$\varphi(n+12) - \varphi(n) = (p+1)(p+5) - (p-1)(p+7) = 12$$
$$\sigma(n+12) - \sigma(n) = (p+3)(p+7) - (p+1)(p+9) = 12$$

## Files

| File | Description |
|---|---|
| `search_flat.py` | Python implementation using a linear sieve, up to ~$10^8$ |
| `search_flat.cpp` | C++ port of the flat sieve, up to ~$10^9$ |
| `search_segmented.cpp` | C++ segmented sieve, runs up to $10^{10}$ with ~12 MB memory |
| `sanity_check.py` | Reads `test_results.json` and verifies all solution properties |

## Implementation

### Flat sieve (`search_flat.py`, `search_flat.cpp`)

1. **Linear sieve** — builds a smallest-prime-factor array `spf[1..N]` in $O(N)$ time and $O(N)$ memory.
2. **$\varphi$ and $\sigma$ via factorization** — for each $n$, factor using `spf` and apply:
   - $\varphi(n) = \prod p_i^{e_i-1}(p_i - 1)$
   - $\sigma(n) = \prod \dfrac{p_i^{e_i+1} - 1}{p_i - 1}$
3. **Search** — scan all composite $n$ in $[2, N)$ checking both jump conditions.

Memory scales as $O(N)$, limiting this approach to roughly $N \leq 10^9$ on typical hardware.

### Segmented sieve (`search_segmented.cpp`)

Processes the range in blocks of ~500K, keeping only three arrays in memory at a time. For each block $[\ell, \ell + B)$:

1. Precompute all primes $p \leq \sqrt{N}$ (~9500 primes for $N = 10^{10}$).
2. For each small prime $p$, sieve its multiples within the block, factoring out $p$ and accumulating $\varphi$ and $\sigma$ contributions multiplicatively.
3. After all small primes, any remaining factor $> 1$ is a large prime; apply its contribution directly.
4. Check composite $n$ in the block for both jump conditions.

Memory is $O(B + \pi(\sqrt{N}))$ regardless of $N$, making $10^{10}$ feasible in ~12 MB. Primality is detected by $\varphi(n) = n - 1$.

## Running

```bash
# Python (up to ~10^8)
python search_flat.py

# C++ flat sieve (up to ~10^9) — compile once, run as needed
g++ -O2 -o search_flat search_flat.cpp
./search_flat

# C++ segmented sieve (up to 10^10)
g++ -O2 -o search_segmented search_segmented.cpp
./search_segmented

# Verify the saved results
python sanity_check.py
```

All programs write results to `test_results.json`.

## Results

Testing has been performed up to $10^{10}$. **38 solutions** were found, all satisfying $n \equiv 65 \pmod{72}$ and all arising from prime quadruplets. The conjecture holds across the full tested range.

## Mathematical References

- **Euler's totient function** $\varphi(n)$: counts integers up to $n$ coprime to $n$
- **Divisor sum function** $\sigma(n)$: sum of all positive divisors of $n$
- **Prime quadruplet**: four primes of the form $(p,\, p+2,\, p+6,\, p+8)$
