#include <nosync/destroy-notifier.h>
#include <nosync/exceptions.h>
#include <nosync/os/synchronized-queue-pushing-executor.h>
#include <utility>

using std::function;
using std::move;
using std::shared_ptr;


namespace nosync::os
{

function<void(function<void()>)> make_synchronized_queue_pushing_executor(
    shared_ptr<synchronized_queue<function<void()>>> out_tasks_queue)
{
    auto null_task_pusher = make_shared_destroy_notifier(
        [out_tasks_queue]() {
            out_tasks_queue->push(nullptr);
        });

    return [out_tasks_queue = move(out_tasks_queue), null_task_pusher](function<void()> task) {
        if (!task) {
            throw_logic_error("Executor got null task");
        }
        out_tasks_queue->push(move(task));
    };
}

}
