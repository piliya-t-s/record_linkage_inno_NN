import random
from collections import defaultdict

class MinHashLSH:
    def __init__(self, num_hash_functions, num_bands, threshold):
        self.num_hash_functions = num_hash_functions
        self.num_bands = num_bands
        self.threshold = threshold
        self.hash_functions = self._generate_hash_functions()
        self.buckets = defaultdict(set)
        self.strings = []

    def _generate_hash_functions(self):
        return [(random.randint(0, 2**32 - 1), random.randint(0, 2**32 - 1)) 
                for _ in range(self.num_hash_functions)]

    def _minhash_signature(self, string):
        shingles = set(string[i:i+3].lower() for i in range(len(string) - 2))
        signature = []
        for a, b in self.hash_functions:
            min_hash = min((a * hash(shingle) + b) % (2**32 - 1) for shingle in shingles)
            signature.append(min_hash)
        return signature

    def add_string(self, string):
        index = len(self.strings)
        self.strings.append(string)
        signature = self._minhash_signature(string)
        for band in range(self.num_bands):
            start = band * (self.num_hash_functions // self.num_bands)
            end = (band + 1) * (self.num_hash_functions // self.num_bands)
            band_signature = tuple(signature[start:end])
            self.buckets[(band, band_signature)].add(index)

    def find_similar_groups(self):
        groups = []
        processed = set()

        for string_index in range(len(self.strings)):
            if string_index in processed:
                continue

            group = set()
            candidates = self.find_similar(self.strings[string_index])

            for candidate in candidates:
                if self.jaccard_similarity(self.strings[string_index], self.strings[candidate]) >= self.threshold:
                    group.add(candidate)

            if len(group) > 1:  # Only add groups with at least 2 members
                groups.append(group)
                processed.update(group)

        return groups

    def find_similar(self, query_string):
        query_signature = self._minhash_signature(query_string)
        candidates = set()
        for band in range(self.num_bands):
            start = band * (self.num_hash_functions // self.num_bands)
            end = (band + 1) * (self.num_hash_functions // self.num_bands)
            band_signature = tuple(query_signature[start:end])
            candidates.update(self.buckets[(band, band_signature)])
        return candidates

    def jaccard_similarity(self, str1, str2):
        set1 = set(str1.lower().split())
        set2 = set(str2.lower().split())
        return len(set1.intersection(set2)) / len(set1.union(set2))

