import unittest

from src.clustering import ClusteringInformation


class TestClusteringInformation(unittest.TestCase):

    def setUp(self):
        self.ci = ClusteringInformation.from_file('test_data/cluster.json')

    def test_get_all_invocation_sets(self):
        self.assertListEqual(self.ci.get_all_invocation_sets(),
                             [
                                [0, 1, 2],
                                [3, 4, 5],
                                [6, 7, 8],
                                [9, 10, 11],
                                [12, 13, 14]
                             ])

    def test_get_all_invocations_in_cluster(self):
        self.assertListEqual(self.ci.get_all_invocations_in_cluster(0),
                             [3, 4, 5, 9, 10, 11])

    def test_get_all_invocations_in_invocation_set(self):
        self.assertListEqual(self.ci.get_all_invocations_in_invocation_set(1),
                             [3, 4, 5])

    def test_get_all_noise_invocation_sets(self):
        self.assertListEqual(self.ci.get_all_noise_invocation_sets(),
                             [2, 4])

    def test_get_all_noise_invocations(self):
        self.assertListEqual(self.ci.get_all_noise_invocations(),
                             [6, 7, 8, 12, 13, 14])

    def test_get_cluster_count(self):
        self.assertEqual(self.ci.get_cluster_count(), 2)

    def test_get_cluster_id_for_invocation(self):
        self.assertEqual(self.ci.get_cluster_id_for_invocation(1), 1)

    def test_get_epsilon(self):
        self.assertEqual(self.ci.get_epsilon(), 0.5)

    def test_get_extra(self):
        self.skipTest("Test missing.")

    def test_get_extra_cluster_metric(self):
        self.skipTest("Test missing.")

    def test_get_instruction_coverage(self):
        self.skipTest("Test missing.")

    def test_get_invocation_count(self):
        self.assertEqual(self.ci.get_invocation_count(), 15,
                         "In total there are 15 invocations, in 5 sets of 3 "
                         "invocations each.")

    def test_get_invocation_count_cluster(self):
        self.assertEqual(self.ci.get_invocation_count(0), 6,
                         "Cluster 0 has 6 invocations, in 2 sets of 3 "
                         "invocations each.")

    def test_get_invocation_coverage(self):
        self.skipTest("Test missing.")

    def test_get_invocation_in_cluster(self):
        self.assertEqual(self.ci.get_invocation_in_cluster(0), 3,
                         "Should return the first invocation of the first set "
                         "of cluster 0, which is invocation 3.")

    def test_get_invocation_set_count(self):
        self.assertEqual(self.ci.get_invocation_set_count(), 2)

    def test_get_noise_invocation_count(self):
        self.assertEqual(self.ci.get_noise_invocation_count(), 6,
                         "There are 6 noise invocations, in 2 sets of 3 "
                         "invocations each.")

    def test_get_noise_invocation_set_count(self):
        self.assertEqual(self.ci.get_noise_invocation_set_count(), 2)

    def test_get_noise_invocations(self):
        invocations = self.ci.get_noise_invocations()
        self.assertEqual(len(invocations), 2,
                         "There should be 2 selected invocations for the "
                         "noise invocations, one from each invocation set.")
        self.assertIn(invocations[0], [6, 7, 8],
                      "Invocation 0 should belong to the 1st invocation set "
                      "and can be either one of invocations 6, 7 or 8.")
        self.assertIn(invocations[1], [12, 13, 14],
                      "Invocation 0 should belong to the 4th invocation set "
                      "and can be either one of invocations 12, 13, 14.")


if __name__ == '__main__':
    unittest.main()
