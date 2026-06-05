#include <bits/stdc++.h>
using namespace std;

static const long long LIMIT = 10000000000LL;
static const long long BLOCK = 1 << 19;

int main() {
    auto t0 = chrono::steady_clock::now();

    long long sqrtL = (long long)sqrt((double)(LIMIT + 13)) + 2;
    while (sqrtL * sqrtL < LIMIT + 13) sqrtL++;

    int sl = (int)(sqrtL + 10);
    vector<bool> isp(sl + 1, true);
    isp[0] = isp[1] = false;
    for (int i = 2; (long long)i * i <= sqrtL + 9; i++)
        if (isp[i])
            for (int j = i * i; j <= sl; j += i)
                isp[j] = false;

    vector<long long> small_primes;
    for (int i = 2; i <= sl; i++)
        if (isp[i]) small_primes.push_back(i);

    long long bsz = BLOCK + 13;
    vector<long long> rem(bsz), phi_v(bsz), sig_v(bsz);

    vector<long long> solutions;
    long long last_pct = -5;

    for (long long lo = 1; lo < LIMIT + 1; lo += BLOCK) {
        long long hi = min(lo + BLOCK + 12, LIMIT + 13);
        long long blk = hi - lo;

        for (long long i = 0; i < blk; i++) {
            rem[i] = lo + i;
            phi_v[i] = 1;
            sig_v[i] = 1;
        }

        for (long long p : small_primes) {
            long long start = ((lo + p - 1) / p) * p;
            for (long long j = start; j < hi; j += p) {
                long long idx = j - lo;
                long long e = 0;
                while (rem[idx] % p == 0) { rem[idx] /= p; e++; }
                if (e == 0) continue;
                long long pe = 1;
                for (long long k = 0; k < e; k++) pe *= p;
                phi_v[idx] *= (pe / p) * (p - 1);
                long long sc = 0, pk = 1;
                for (long long k = 0; k <= e; k++) { sc += pk; pk *= p; }
                sig_v[idx] *= sc;
            }
        }

        for (long long i = 0; i < blk; i++) {
            if (rem[i] > 1) {
                long long q = rem[i];
                phi_v[i] *= q - 1;
                sig_v[i] *= q + 1;
            }
        }

        long long n_lo = max(2LL, lo);
        long long n_hi = min(lo + BLOCK, LIMIT);

        for (long long n = n_lo; n < n_hi; n++) {
            long long idx = n - lo;
            if (phi_v[idx] == n - 1) continue;
            if (phi_v[idx + 12] - phi_v[idx] != 12) continue;
            if (sig_v[idx + 12] - sig_v[idx] != 12) continue;
            solutions.push_back(n);
        }

        long long pct = lo * 100 / LIMIT;
        if (pct / 5 > last_pct / 5) {
            last_pct = pct;
            auto elapsed = chrono::duration_cast<chrono::seconds>(
                chrono::steady_clock::now() - t0).count();
            printf("Progress: %lld%%  (%llds elapsed)\n", pct, elapsed);
            fflush(stdout);
        }
    }

    auto total_s = chrono::duration_cast<chrono::seconds>(
        chrono::steady_clock::now() - t0).count();
    printf("\nDone in %llds\n\n", total_s);

    printf("Total solutions in [2, %lld): %d\n", LIMIT, (int)solutions.size());

    set<long long> residues;
    for (long long n : solutions) residues.insert(n % 72);
    printf("Distinct residues mod 72: {");
    bool first = true;
    for (long long r : residues) { if (!first) printf(", "); printf("%lld", r); first = false; }
    printf("}\n\n");

    printf("Table 1 -- all composite n with both jump conditions:\n\n");
    printf("%8s %18s %8s %8s %24s %10s\n",
        "p", "n=p(p+8)", "p+2", "p+6", "n+12=(p+2)(p+6)", "n mod 72");
    printf("%s\n", string(82, '-').c_str());

    bool all_quad = true, all_65 = true;

    for (long long n : solutions) {
        long long tmp = n;
        vector<pair<long long, int>> factors;
        for (long long p : small_primes) {
            if (p * p > tmp) break;
            if (tmp % p == 0) {
                int e = 0;
                while (tmp % p == 0) { tmp /= p; e++; }
                factors.push_back({p, e});
            }
        }
        if (tmp > 1) factors.push_back({tmp, 1});

        vector<long long> pf;
        for (auto& [p, e] : factors)
            for (int i = 0; i < e; i++) pf.push_back(p);

        if (pf.size() != 2 || pf[1] - pf[0] != 8) {
            printf("  Anomaly. n = %lld unexpected factorization\n", n);
            all_quad = false;
            continue;
        }
        long long p = pf[0];

        bool quad = (p <= sl && isp[p]) &&
                    (p + 2 <= sl && isp[p + 2]) &&
                    (p + 6 <= sl && isp[p + 6]) &&
                    (p + 8 <= sl && isp[p + 8]);
        if (!quad) {
            printf("  Anomaly. n=%lld=%lld*%lld not prime quadruplet\n", n, p, p + 8);
            all_quad = false;
            continue;
        }

        if (n + 12 != (p + 2) * (p + 6)) {
            printf("  Identity failed. %lld+12 != (%lld+2)*(%lld+6)\n", n, p, p);
            continue;
        }

        long long r = n % 72;
        if (r != 65) all_65 = false;
        printf("%8lld %18lld %8lld %8lld %24lld %10lld\n",
            p, n, p + 2, p + 6, n + 12, r);
    }

    printf("\n  All solutions arise from prime quadruplets: %s\n", all_quad ? "True" : "False");
    printf("  All solutions are 65 (mod 72):              %s\n", all_65 ? "True" : "False");
    if (all_quad && all_65) printf("\nConsistent.\n");

    FILE* jf = fopen("test_results.json", "w");
    fprintf(jf, "{\n  \"test_results\": {\n");
    fprintf(jf, "    \"limit\": %lld,\n", LIMIT);
    fprintf(jf, "    \"total_solutions\": %d,\n", (int)solutions.size());
    fprintf(jf, "    \"distinct_residues_mod_72\": [");
    { bool f = true; for (long long r : residues) { if (!f) fprintf(jf, ", "); fprintf(jf, "%lld", r); f = false; } }
    fprintf(jf, "],\n    \"solutions\": [\n");
    for (int i = 0; i < (int)solutions.size(); i++) {
        long long n = solutions[i], tmp = n, p = -1;
        for (long long pp : small_primes) {
            if (pp * pp > tmp) break;
            if (tmp % pp == 0) { p = pp; break; }
        }
        if (p < 0) p = tmp;
        fprintf(jf,
            "      {\"p\": %lld, \"n\": %lld, \"p_plus_2\": %lld, \"p_plus_6\": %lld,"
            " \"n_plus_12\": %lld, \"n_mod_72\": %lld}%s\n",
            p, n, p + 2, p + 6, n + 12, n % 72,
            i + 1 < (int)solutions.size() ? "," : "");
    }
    fprintf(jf, "    ],\n    \"verification\": {\n");
    fprintf(jf, "      \"all_from_quadruplet\": %s,\n", all_quad ? "true" : "false");
    fprintf(jf, "      \"all_residue_65\": %s,\n", all_65 ? "true" : "false");
    fprintf(jf, "      \"consistent_with_theorems\": %s\n",
        (all_quad && all_65) ? "true" : "false");
    fprintf(jf, "    }\n  }\n}\n");
    fclose(jf);

    return 0;
}
