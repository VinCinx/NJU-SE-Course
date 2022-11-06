package program;

import org.junit.Before;
import org.junit.Test;

import java.io.*;
import java.util.ArrayList;

import static org.junit.Assert.assertEquals;
import static org.junit.Assert.assertTrue;

public class MaxTest {
    private ArrayList<String> logList;

    @Before
    public void before() {
        Log.init();
    }

    @Test
    public void test1() throws IOException {
        logList = file2list("test/max_test_1.txt");
        Max max = new Max();
        int res = max.max(65, 40);
        assertEquals(65, res);
        assertTrue(Log.isSameWith(logList));
    }

    @Test
    public void test2() throws IOException {
        logList = file2list("test/max_test_2.txt");
        Max max = new Max();
        int res = max.max(10, 10);
        assertEquals(10, res);
        assertTrue(Log.isSameWith(logList));
    }

    @Test
    public void test3() throws IOException {
        logList = file2list("test/max_test_3.txt");
        Max max = new Max();
        int res = max.max(10, 50);
        assertEquals(50, res);
        assertTrue(Log.isSameWith(logList));
    }

    private static ArrayList<String> file2list(String path) throws IOException {
        File filename = new File(path);
        InputStreamReader reader = new InputStreamReader(
                new FileInputStream(filename));
        BufferedReader br = new BufferedReader(reader);
        ArrayList<String> ans = new ArrayList<>();
        String line = "";
        while ((line = br.readLine()) != null) {
            if (line != null && !line.equals("")) {
                ans.add(line);
            }
        }
        return ans;
    }

}
