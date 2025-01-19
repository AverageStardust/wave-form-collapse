import { child_process } from "vite-plugin-child-process"

export default {
    plugins: [child_process({
        name: "make",
        command: ["make"],
        watch: [/src/],
    })
    ],
};