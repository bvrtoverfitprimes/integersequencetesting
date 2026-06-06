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
| `search_flat.py` | Original Python implementation using a linear sieve, up to ~$10^8$ |
| `search_flat.cpp` | C++ port of the flat sieve, up to $10^9$ |
| `search_segmented.cpp` | C++ segmented sieve, runs up to $10^{10}$ with ~12 MB memory |
| `search_segmented_fast.cpp` | Optimized segmented sieve with `e=1` fast path and `__builtin_ctzll` for $p=2$, used for $10^{11}$ |
| `search_omp.cpp` | OpenMP parallel version used on AWS for $10^{12}$ |
| `sanity_check.py` | Reads `test_results.json` and verifies all solution properties |

## Implementation

### Flat sieve (`search_flat.py`, `search_flat.cpp`)

1. **Linear sieve** — builds a smallest-prime-factor array `spf[1..N]` in $O(N)$ time and $O(N)$ memory.
2. **$\varphi$ and $\sigma$ via factorization** — for each $n$, factor using `spf` and apply:
   - $\varphi(n) = \prod p_i^{e_i-1}(p_i - 1)$
   - $\sigma(n) = \prod \dfrac{p_i^{e_i+1} - 1}{p_i - 1}$
3. **Search** — scan all composite $n$ in $[2, N)$ checking both jump conditions.

Memory scales as $O(N)$, limiting this approach to roughly $N \leq 10^9$ on typical hardware.

### Segmented sieve (`search_segmented.cpp`, `search_segmented_fast.cpp`)

Processes the range in blocks of ~500K, keeping only three arrays in memory at a time. For each block $[\ell, \ell + B)$:

1. Precompute all primes $p \leq \sqrt{N}$.
2. For each small prime $p$, sieve its multiples within the block, factoring out $p$ and accumulating $\varphi$ and $\sigma$ contributions multiplicatively.
3. After all small primes, any remaining factor $> 1$ is a large prime; apply its contribution directly.
4. Check composite $n$ in the block for both jump conditions.

Memory is $O(B + \pi(\sqrt{N})) \approx 12\,\text{MB}$ regardless of $N$. Primality is detected by $\varphi(n) = n - 1$. The fast variant special-cases $e=1$ (the common case) to avoid inner loops, and uses `__builtin_ctzll` for $p=2$.

### OpenMP parallel version (`search_omp.cpp`)

Identical algorithm to the segmented sieve but each block is processed independently across threads using OpenMP. Run on AWS EC2 (`c7i.8xlarge`, 32 vCPUs) for the $10^{12}$ search.

## Running

```bash
# Python (up to ~10^8)
python search_flat.py

# C++ flat sieve (up to ~10^9)
g++ -O2 -o search_flat search_flat.cpp
./search_flat

# C++ segmented sieve (up to 10^10 / 10^11)
g++ -O2 -o search_segmented search_segmented.cpp
./search_segmented

# OpenMP parallel version (10^12, intended for multi-core machines)
g++ -O2 -fopenmp -o search_omp search_omp.cpp
./search_omp

# Verify saved results
python sanity_check.py
```

All programs write results to `test_results.json`.

## Results

| Limit | Solutions | Consistent | Time | Hardware |
|---|---|---|---|---|
| $10^8$ | 12 | Yes | ~seconds | Local (Python) |
| $10^9$ | 26 | Yes | ~minutes | Local (C++) |
| $10^{10}$ | 38 | Yes | 1396s | Local (C++) |
| $10^{11}$ | ~80 | Yes | ~13800s | Local (C++) |
| $10^{12}$ | **166** | **Yes** | **5536s** | AWS c7i.8xlarge (32 vCPUs) |

All 166 solutions satisfy $n \equiv 65 \pmod{72}$ and all arise from prime quadruplets. The conjecture holds across the full tested range up to $10^{12}$.

## OEIS

This project directly computes **OEIS A056777**: composite numbers $k$ such that both $\varphi(k+12) = \varphi(k) + 12$ and $\sigma(k+12) = \sigma(k) + 12$. The OEIS lists 23 known terms (up to ~$10^9$); our search confirms all 23 and extends the sequence to **166 terms up to $10^{12}$**, all consistent with the conjecture stated by Jud McCranie (2000) that every term arises from a prime quadruplet, and with Ralf Stephan's observation (2003) that all terms are congruent to $65 \pmod{72}$.

The connection to prime quadruplets links this sequence to **OEIS A007530** (values of $p$ where $(p, p+2, p+6, p+8)$ are all prime).

## Mathematical References

- **Euler's totient function** $\varphi(n)$: counts integers up to $n$ coprime to $n$
- **Divisor sum function** $\sigma(n)$: sum of all positive divisors of $n$
- **Prime quadruplet**: four primes of the form $(p,\, p+2,\, p+6,\, p+8)$
