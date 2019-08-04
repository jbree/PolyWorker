# Parallel Task Synchronization with PolyWorker

Polyworker is a parallel task-runner which keeps output in the same order as
input.

The user defines work input, work output, and a function which performs the work
to convert input to output. PolyWorker performs the work and provides the
output, in-order and when-available, to the user.

## PolyWorker

General flow of operations is as follows

PolyWorker spawns *n* `WorkerThreads` to accomplish work.
PolyWorker spawns 1 `SynchronizerThread` to wait for and post finished work.

As work becomes available, the user adds it to the PolyWorker via `addWork()`.
Invocations of `addWork()` should be ordered by the user for work output to be
ordered.

The input work is wrapped in a `Work` object and pushed it onto the queue of
available work. When a `WorkerThread` is ready to perform a piece of work, they
invoke `PolyWorker.getWork()`. This method blocks until work is available. When
work is available, the method transfers the work from the available work queue
to the in-progress work queue. It's important that this operation is atomic,
preventing any incidental reordering.

### Worker Thread

Each `WorkerThread` waits for available work on the available queue. Once work
is available, it must be taken from the available queue and added to the
PolyWorker's in-progress queue in one atomic operation, guaranteeing that all
work is kept in-order.

Having handled the synchronization bookkeeping, the `WorkerThread` now invokes
the user-provided callback to perform the work.

### Synchronizer Thread

The synchronizer thread waits for work to be added to the `workInProgress_`
queue. Once the first piece of work has been identified, the synchronizer waits
on that `Work` item's flag to be posted, indicating completion. At this time,
the synchronizer calls the user-specified callback to indicate that the next
piece of work has been completed.