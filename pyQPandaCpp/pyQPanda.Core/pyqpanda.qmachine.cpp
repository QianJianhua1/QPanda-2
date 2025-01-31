#include <math.h>
#include "pybind11/pybind11.h"
#include "pybind11/stl.h"
#include "pybind11/complex.h"
#include "pybind11/functional.h"
#include "pybind11/chrono.h"
#include "Core/Core.h"
#include "Core/QuantumMachine/SingleAmplitudeQVM.h"
#include "Core/QuantumMachine/PartialAmplitudeQVM.h"
#include "Core/QuantumMachine/QCloudMachine.h"
#include "Core/VirtualQuantumProcessor/NoiseQPU/NoiseModel.h"
USING_QPANDA
using namespace std;
namespace py = pybind11;
using namespace pybind11::literals;

//template<>
//struct py::detail::type_caster<QVec>  : py::detail::list_caster<QVec, Qubit*> {};

void init_quantum_machine(py::module &m)
{
    py::enum_<QMachineType>(m, "QMachineType")
        .value("CPU", QMachineType::CPU)
        .value("GPU", QMachineType::GPU)
        .value("CPU_SINGLE_THREAD", QMachineType::CPU_SINGLE_THREAD)
        .value("NOISE", QMachineType::NOISE)
        .export_values();

    py::enum_<NOISE_MODEL>(m, "NoiseModel")
        .value("DAMPING_KRAUS_OPERATOR", NOISE_MODEL::DAMPING_KRAUS_OPERATOR)
        .value("DECOHERENCE_KRAUS_OPERATOR", NOISE_MODEL::DECOHERENCE_KRAUS_OPERATOR)
        .value("DEPHASING_KRAUS_OPERATOR", NOISE_MODEL::DEPHASING_KRAUS_OPERATOR)
        .value("PAULI_KRAUS_MAP", NOISE_MODEL::PAULI_KRAUS_MAP)
        .value("BITFLIP_KRAUS_OPERATOR", NOISE_MODEL::BITFLIP_KRAUS_OPERATOR)
        .value("DEPOLARIZING_KRAUS_OPERATOR", NOISE_MODEL::DEPOLARIZING_KRAUS_OPERATOR)
        .value("BIT_PHASE_FLIP_OPRATOR", NOISE_MODEL::BIT_PHASE_FLIP_OPRATOR)
        .value("PHASE_DAMPING_OPRATOR", NOISE_MODEL::PHASE_DAMPING_OPRATOR)
        ;

    py::enum_<GateType>(m, "GateType")
        .value("P0_GATE", GateType::P0_GATE)
        .value("P1_GATE", GateType::P1_GATE)
        .value("PAULI_X_GATE", GateType::PAULI_X_GATE)
        .value("PAULI_Y_GATE", GateType::PAULI_Y_GATE)
        .value("PAULI_Z_GATE", GateType::PAULI_Z_GATE)
        .value("X_HALF_PI", GateType::X_HALF_PI)
        .value("Y_HALF_PI", GateType::Y_HALF_PI)
        .value("Z_HALF_PI", GateType::Z_HALF_PI)
        .value("HADAMARD_GATE", GateType::HADAMARD_GATE)
        .value("T_GATE", GateType::T_GATE)
        .value("S_GATE", GateType::S_GATE)
        .value("RX_GATE", GateType::RX_GATE)
        .value("RY_GATE", GateType::RY_GATE)
        .value("RZ_GATE", GateType::RZ_GATE)
        .value("U1_GATE", GateType::U1_GATE)
        .value("U2_GATE", GateType::U2_GATE)
        .value("U3_GATE", GateType::U3_GATE)
        .value("U4_GATE", GateType::U4_GATE)
        .value("CU_GATE", GateType::CU_GATE)
        .value("CNOT_GATE", GateType::CNOT_GATE)
        .value("CZ_GATE", GateType::CZ_GATE)
        .value("CPHASE_GATE", GateType::CPHASE_GATE)
        .value("ISWAP_THETA_GATE", GateType::ISWAP_THETA_GATE)
        .value("ISWAP_GATE", GateType::ISWAP_GATE)
        .value("SQISWAP_GATE", GateType::SQISWAP_GATE)
        .value("SWAP_GATE", GateType::SWAP_GATE)
        .value("TWO_QUBIT_GATE", GateType::TWO_QUBIT_GATE)
        .value("P00_GATE", GateType::P00_GATE)
        .value("P11_GATE", GateType::P11_GATE)
        .value("TOFFOLI_GATE", GateType::TOFFOLI_GATE)
        .value("ORACLE_GATE", GateType::ORACLE_GATE)
        .value("I_GATE", GateType::I_GATE)
		.value("BARRIER_GATE", GateType::BARRIER_GATE)
		.value("RPHI_GATE", GateType::RPHI_GATE)
        ;

    Qubit*(QVec::*qvec_subscript_cbit_size_t)(size_t) const = &QVec::operator[];
    Qubit*(QVec::*qvec_subscript_cc)(ClassicalCondition&) = &QVec::operator[];

    py::class_<QVec>(m, "QVec")
        .def(py::init<>())
        .def(py::init<std::vector<Qubit *> &>())
        .def(py::init<const QVec &>())
        .def(py::init<Qubit *>())
        .def("__getitem__", [](QVec & self, int num) {return self[num]; }, py::return_value_policy::reference)
        .def("__getitem__", qvec_subscript_cc, py::return_value_policy::reference)
        .def("__len__", &QVec::size)
		.def("to_list", [](QVec &self) {
			std::vector<Qubit*> q_list;
			for (const auto& item : self)
			{
				q_list.push_back(item);
			}
			return q_list;
		}, py::return_value_policy::reference)
        .def("append", [](QVec & self, Qubit * qubit) {self.push_back(qubit); })
        .def("pop", [](QVec & self) {self.pop_back(); });

    Qubit*(QuantumMachine::*qalloc)() = &QuantumMachine::allocateQubit;
    ClassicalCondition(QuantumMachine::*cAlloc)() = &QuantumMachine::allocateCBit;
    ClassicalCondition(QuantumMachine::*calloc_by_address)(size_t) = &QuantumMachine::allocateCBit;
    QVec(QuantumMachine::*qallocMany)(size_t) = &QuantumMachine::allocateQubits;
    std::vector<ClassicalCondition>(QuantumMachine::*callocMany)(size_t) = &QuantumMachine::allocateCBits;
    void (QuantumMachine::*free_qubit)(Qubit *) = &QuantumMachine::Free_Qubit;
    void (QuantumMachine::*free_qubits)(QVec&) = &QuantumMachine::Free_Qubits;
    void (QuantumMachine::*free_cbit)(ClassicalCondition &) = &QuantumMachine::Free_CBit;
    void (QuantumMachine::*free_cbits)(std::vector<ClassicalCondition> &) = &QuantumMachine::Free_CBits;
    QMachineStatus * (QuantumMachine::*get_status)() const = &QuantumMachine::getStatus;
    size_t(QuantumMachine::*get_allocate_qubit)() = &QuantumMachine::getAllocateQubit;
    size_t(QuantumMachine::*get_allocate_CMem)() = &QuantumMachine::getAllocateCMem;
    void (QuantumMachine::*_finalize)() = &QuantumMachine::finalize;
    Qubit* (QuantumMachine::*allocate_qubit_through_phy_address)(size_t) = &QuantumMachine::allocateQubitThroughPhyAddress;
    Qubit* (QuantumMachine::*allocate_qubit_through_vir_address)(size_t) = &QuantumMachine::allocateQubitThroughVirAddress;
    std::map<GateType, size_t>(QuantumMachine::*get_gate_time_map)() const = &QuantumMachine::getGateTimeMap;
    bool(QuantumMachine::*swap_qubit_physical_address)(Qubit*, Qubit*) = &QuantumMachine::swapQubitPhysicalAddress;
	size_t(QuantumMachine::* get_allocate_qubits)(QVec&) = &QuantumMachine::get_allocate_qubits;
	size_t(QuantumMachine::* get_allocate_cbits)(std::vector<ClassicalCondition>&) = &QuantumMachine::get_allocate_cbits;
	double(QuantumMachine::* get_expectation_shots)(QProg, const QHamiltonian&, const QVec&, int) = &QuantumMachine::get_expectation;
	double(QuantumMachine::* get_expectation)(QProg, const QHamiltonian&, const QVec&) = &QuantumMachine::get_expectation;
    size_t(QuantumMachine::* get_processed_qgate_num)() = &QuantumMachine::get_processed_qgate_num;

    py::class_<QuantumMachine>(m, "QuantumMachine")
        .def("set_configure", [](QuantumMachine &qvm, size_t max_qubit, size_t max_cbit)
            {
                Configuration config = { max_qubit, max_cbit };
                qvm.setConfig(config);
            }, "set QVM max qubit and max cbit", "max_qubit"_a, "max_cbit"_a)
        .def("finalize", &QuantumMachine::finalize, "finalize")
        .def("get_qstate", &QuantumMachine::getQState, "getState", py::return_value_policy::automatic)
        .def("qAlloc", qalloc, "Allocate a qubit", py::return_value_policy::reference)
        .def("qAlloc_many", [](QuantumMachine & self, size_t num) { 
				vector<Qubit *> temp = self.allocateQubits(num); return temp; },
            "Allocate a list of qubits", "n_qubit"_a, py::return_value_policy::reference)
        .def("cAlloc", cAlloc, "Allocate a cbit", py::return_value_policy::reference)
        .def("cAlloc_many", callocMany, "Allocate a list of cbits", "n_cbit"_a, py::return_value_policy::reference)
        .def("qFree", free_qubit, "Free a qubit")
        .def("qFree_all", free_qubits, "qubit_list"_a, "Free a list of qubits")
        .def("cFree", free_cbit, "Free a cbit")
        .def("cFree_all", free_cbits, "cbit_list"_a, "Free a list of cbits")

        /*will delete*/
        .def("getAllocateQubitNum", get_allocate_qubit, "getAllocateQubitNum", py::return_value_policy::reference)
        .def("getAllocateCMem", get_allocate_CMem, "getAllocateCMem", py::return_value_policy::reference)
        .def("getStatus", get_status, "get the status(ptr) of the quantum machine", py::return_value_policy::reference)

        /* new interface */
        .def("cAlloc", calloc_by_address, "Allocate a cbit", "cbit"_a, py::return_value_policy::reference)
        .def("get_status", get_status, "get the status(ptr) of the quantum machine", py::return_value_policy::reference)
        .def("get_allocate_qubit_num", get_allocate_qubit, "getAllocateQubitNum", py::return_value_policy::reference)\
        .def("get_allocate_cmem_num", get_allocate_CMem, "getAllocateCMem", py::return_value_policy::reference)\
        .def("allocate_qubit_through_phy_address", allocate_qubit_through_phy_address, "address"_a, py::return_value_policy::reference)
        .def("allocate_qubit_through_vir_address", allocate_qubit_through_vir_address, "address"_a, py::return_value_policy::reference)
        .def("get_gate_time_map", get_gate_time_map, py::return_value_policy::reference)
        .def("get_allocate_qubits", get_allocate_qubits, "qubit vector"_a,  py::return_value_policy::reference)
		.def("get_allocate_cbits", get_allocate_cbits, "cbit vector"_a,  py::return_value_policy::reference)
		.def("get_expectation", get_expectation, "quantum prog"_a, "hamiltonian"_a, "qubit vector"_a, py::return_value_policy::reference)
		.def("get_expectation", get_expectation_shots, "quantum prog"_a, "hamiltonian"_a, "qubit vector"_a, "shots"_a, py::return_value_policy::reference)
        .def("get_processed_qgate_num", get_processed_qgate_num, py::return_value_policy::reference)
        .def("async_run", &QuantumMachine::async_run, "program"_a, py::return_value_policy::reference)
        .def("is_async_finished", &QuantumMachine::is_async_finished, py::return_value_policy::reference)
        .def("get_async_result", &QuantumMachine::get_async_result, py::return_value_policy::reference)
        .def("directly_run", &QuantumMachine::directlyRun, "program"_a, py::return_value_policy::reference)
        .def("run_with_configuration", [](QuantumMachine &qvm, QProg & prog, vector<ClassicalCondition> & cc_vector, py::dict param)
            {
                py::object json = py::module::import("json");
                py::object dumps = json.attr("dumps");
                auto json_string = std::string(py::str(dumps(param)));
                rapidjson::Document doc;
                auto & alloc = doc.GetAllocator();
                doc.Parse(json_string.c_str());
                return qvm.runWithConfiguration(prog, cc_vector, doc);
            }, "program"_a, "cbit_list"_a, "data"_a, py::return_value_policy::automatic)

        .def("run_with_configuration", [](QuantumMachine &qvm, QProg & prog, vector<ClassicalCondition> & cc_vector, int shots)
            {return qvm.runWithConfiguration(prog, cc_vector, shots); }, "program"_a, "cbit_list"_a, "data"_a, py::return_value_policy::automatic)
		.def("run_with_configuration", [](QuantumMachine& qvm, QProg& prog, vector<int>& cbit_addrs, int shots)
			{return qvm.runWithConfiguration(prog, cbit_addrs, shots); }, "program"_a, "cbit_addr_list"_a, "data"_a, py::return_value_policy::automatic);


#define DEFINE_IDEAL_QVM(class_name)\
    py::class_<class_name,QuantumMachine>(m, #class_name)\
		.def("set_configure", [](class_name &qvm, size_t max_qubit, size_t max_cbit) \
            { Configuration config = { max_qubit, max_cbit }; \
	            qvm.setConfig(config); \
            }, "set QVM max qubit and max cbit", "max_qubit"_a, "max_cbit"_a) \
        .def(py::init<>())\
        .def("finalize", &class_name::finalize, "finalize")\
        .def("get_qstate", &class_name::getQState, "getState",py::return_value_policy::automatic)\
        .def("qAlloc", qalloc, "Allocate a qubit", py::return_value_policy::reference)\
        .def("qAlloc_many", [](QuantumMachine & self,size_t num) { std::vector<Qubit *> temp = self.allocateQubits(num); return temp;},\
			 "Allocate a list of qubits", "n_qubit"_a, py::return_value_policy::reference)\
        .def("cAlloc", cAlloc, "Allocate a cbit", py::return_value_policy::reference)\
        .def("cAlloc_many", callocMany, "Allocate a list of cbits", "n_cbit"_a, py::return_value_policy::reference)\
        .def("qFree", free_qubit, "Free a qubit")\
        .def("qFree_all", free_qubits, "qubit_list"_a, "Free a list of qubits")\
        .def("cFree", free_cbit, "Free a cbit")\
        .def("cFree_all", free_cbits, "cbit_list"_a,"Free a list of cbits")\
        .def("getStatus", get_status, "get the status(ptr) of the quantum machine",py::return_value_policy::reference)\
        /*will delete*/\
        .def("initQVM", &class_name::init, "init quantum virtual machine")\
        .def("getAllocateQubitNum", get_allocate_qubit, "getAllocateQubitNum", py::return_value_policy::reference)\
        .def("getAllocateCMem", get_allocate_CMem, "getAllocateCMem", py::return_value_policy::reference)\
         /* new interface */\
        .def("init_qvm", &class_name::init, "init quantum virtual machine")\
        .def("init_state", [](class_name& self, const QStat & state, const QVec &qlist){ \
            self.initState(state, qlist); \
        },py::arg("state") = QStat(), py::arg("qlist") = QVec() \
        ,py::return_value_policy::reference)\
        .def("get_allocate_qubit_num", get_allocate_qubit, "getAllocateQubitNum", py::return_value_policy::reference)\
        .def("get_allocate_cmem_num", get_allocate_CMem, "getAllocateCMem", py::return_value_policy::reference)\
		.def("directly_run", &class_name::directlyRun, \
			"directly_run a prog", "program"_a, py::return_value_policy::reference)\
        .def("get_processed_qgate_num", &class_name::get_processed_qgate_num, py::return_value_policy::reference)\
        .def("async_run", &class_name::async_run, "program"_a, py::return_value_policy::reference)\
        .def("is_async_finished", &class_name::is_async_finished, py::return_value_policy::reference)\
        .def("get_async_result", &class_name::get_async_result, py::return_value_policy::reference)\
        .def("run_with_configuration", [](class_name &qvm, QProg &prog, vector<ClassicalCondition> & cc_vector, py::dict param)\
			{\
				py::object json = py::module::import("json");\
				py::object dumps = json.attr("dumps");\
				auto json_string = std::string(py::str(dumps(param)));\
				rapidjson::Document doc;\
				auto & alloc = doc.GetAllocator();\
				doc.Parse(json_string.c_str());\
				return qvm.runWithConfiguration(prog, cc_vector, doc);\
			},"program"_a, "cbit_list"_a, "data"_a,py::return_value_policy::automatic)\
		.def("run_with_configuration", [](class_name &qvm, QProg & prog, vector<ClassicalCondition> & cc_vector, int shots) \
			{ return qvm.runWithConfiguration(prog, cc_vector, shots);},"program"_a, "cbit_list"_a, "data"_a,py::return_value_policy::automatic)\
        .def("run_with_configuration", [](class_name& qvm, QProg& prog, vector<int>& cbit_addrs, int shots)\
			{return qvm.runWithConfiguration(prog, cbit_addrs, shots); }, "program"_a, "cbit_addr_list"_a, "data"_a, py::return_value_policy::automatic)\
        .def("pmeasure", &class_name::PMeasure, "qubit_list"_a, "select_max"_a = -1, "Get the probability distribution over qubits", py::return_value_policy::reference)\
        .def("pmeasure_no_index", &class_name::PMeasure_no_index, "qubit_list"_a,"Get the probability distribution over qubits", py::return_value_policy::reference)\
        .def("get_prob_tuple_list", &class_name::getProbTupleList, "qubit_list"_a, "select_max"_a = -1,py::return_value_policy::reference)\
        .def("get_prob_list", &class_name::getProbList, "qubit_list"_a, "select_max"_a = -1,py::return_value_policy::reference)\
        .def("get_prob_dict", &class_name::getProbDict, "qubit_list"_a, "select_max"_a = -1,py::return_value_policy::reference)\
        .def("prob_run_tuple_list", [](class_name &qvm, QProg prog, QVec qubit_vect, int select_max= -1)\
            {return qvm.probRunTupleList(prog, qubit_vect, select_max);},"program"_a, "qubit_list"_a, "select_max"_a = -1,py::return_value_policy::automatic)\
		.def("prob_run_tuple_list", [](class_name& qvm, QProg prog, std::vector<int> qubit_addr_vect, int select_max = -1)\
			{return qvm.probRunTupleList(prog, qubit_addr_vect, select_max); }, "program"_a, "qubit addr list"_a, "select_max"_a = -1, py::return_value_policy::automatic)\
        .def("prob_run_list", [](class_name &qvm, QProg prog, QVec qubit_vect, int select_max= -1)\
            {return qvm.probRunList(prog, qubit_vect, select_max);},"program"_a, "qubit_list"_a, "select_max"_a = -1,py::return_value_policy::automatic)\
		.def("prob_run_list", [](class_name& qvm, QProg prog, std::vector<int> qubit_addr_vect, int select_max = -1)\
			{return qvm.probRunList(prog, qubit_addr_vect, select_max); }, "program"_a, "qubit addr list"_a, "select_max"_a = -1, py::return_value_policy::automatic)\
        .def("prob_run_dict", [](class_name &qvm, QProg prog, QVec qubit_vect, int select_max= -1)\
            {return qvm.probRunDict(prog, qubit_vect, select_max);},"program"_a, "qubit_list"_a, "select_max"_a = -1,py::return_value_policy::automatic)\
		.def("prob_run_dict", [](class_name& qvm, QProg prog, std::vector<int> qubit_addr_vect, int select_max = -1)\
			{return qvm.probRunDict(prog, qubit_addr_vect, select_max); }, "program"_a, "qubit addr list"_a, "select_max"_a = -1, py::return_value_policy::automatic)\
        .def("quick_measure", &class_name::quickMeasure, "qubit_list"_a, "shots"_a,py::return_value_policy::reference)\

    DEFINE_IDEAL_QVM(CPUQVM);
    DEFINE_IDEAL_QVM(CPUSingleThreadQVM);

#ifdef USE_CUDA
    DEFINE_IDEAL_QVM(GPUQVM);
#endif // USE_CUDA

    py::class_<NoiseQVM, QuantumMachine>(m, "NoiseQVM")
        .def(py::init<>())
        .def("get_qstate", &NoiseQVM::getQState, "getState", py::return_value_policy::automatic)
        .def("init_qvm", [](NoiseQVM &self) {self.init(); }, "init quantum virtual machine")

        .def("set_noise_model", [](NoiseQVM& self, const NOISE_MODEL &model, const GateType &type, double prob){
                self.set_noise_model(model, type, prob);})
		.def("set_noise_model", [](NoiseQVM& self, const NOISE_MODEL &model, const std::vector<GateType> &types, double prob) {
					self.set_noise_model(model, types, prob); })
        .def("set_noise_model", [](NoiseQVM& self, const NOISE_MODEL &model, const GateType &type, double prob, const QVec &qubits){
                self.set_noise_model(model, type, prob, qubits);
             })
		.def("set_noise_model", [](NoiseQVM& self, const NOISE_MODEL &model, const std::vector<GateType> &types, double prob, const QVec &qubits) {
				 self.set_noise_model(model, types, prob, qubits);
			 })
        .def("set_noise_model", [](NoiseQVM& self, const NOISE_MODEL &model, const GateType &type, double prob, const std::vector<QVec> &qubits){
                self.set_noise_model(model, type, prob, qubits);
             })

        .def("set_noise_model", [](NoiseQVM& self, const NOISE_MODEL &model, const GateType &type, double T1, double T2, double t_gate){
                self.set_noise_model(model, type, T1, T2, t_gate);
             })
		.def("set_noise_model", [](NoiseQVM& self, const NOISE_MODEL &model, const std::vector<GateType> &types, double T1, double T2, double t_gate) {
				 self.set_noise_model(model, types, T1, T2, t_gate);
			 })
        .def("set_noise_model", [](NoiseQVM& self, const NOISE_MODEL &model, const GateType &type, double T1, double T2, double t_gate,
                                   const QVec &qubits){
                self.set_noise_model(model, type, T1, T2, t_gate, qubits);
             })
		.def("set_noise_model", [](NoiseQVM& self, const NOISE_MODEL &model, const std::vector<GateType> &types, double T1, double T2, double t_gate,
					 const QVec &qubits) {
				 self.set_noise_model(model, types, T1, T2, t_gate, qubits);
			 })
        .def("set_noise_model", [](NoiseQVM& self, const NOISE_MODEL &model, const GateType &type, double T1, double T2, double t_gate,
                                   const std::vector<QVec> &qubits){
                self.set_noise_model(model, type, T1, T2, t_gate, qubits);
             })

        .def("set_measure_error", [](NoiseQVM& self, const NOISE_MODEL &model, double prob, const QVec &qubits){
                self.set_measure_error(model, prob, qubits);
            },
            py::arg("model"), py::arg("prob"), py::arg("qubits") = QVec())
        .def("set_measure_error", [](NoiseQVM &self, const NOISE_MODEL &model, double T1, double T2, double t_gate,
             const QVec &qubits){
                self.set_measure_error(model, T1, T2, t_gate, qubits);
            },
            py::arg("model"), py::arg("T1"), py::arg("T2"), py::arg("t_gate"), py::arg("qubits") = QVec())


        .def("set_mixed_unitary_error", [](NoiseQVM &self, const GateType &type, const std::vector<QStat> &unitary_matrices,
             const std::vector<double> &probs){
                self.set_mixed_unitary_error(type, unitary_matrices, probs);
            })
        .def("set_mixed_unitary_error", [](NoiseQVM &self, const GateType &type, const std::vector<QStat> &unitary_matrices,
             const std::vector<double> &probs, const QVec &qubits){
                self.set_mixed_unitary_error(type, unitary_matrices, probs, qubits);
            })
        .def("set_mixed_unitary_error", [](NoiseQVM &self, const GateType &type, const std::vector<QStat> &unitary_matrices,
             const std::vector<double> &probs, const std::vector<QVec> &qubits){
                self.set_mixed_unitary_error(type, unitary_matrices, probs, qubits);
            })

        .def("set_reset_error", [](NoiseQVM &self, double p0, double p1, const QVec &qubits = {}){
                self.set_reset_error(p0, p1, qubits);
            },
            py::arg("p0"), py::arg("p1"), py::arg("qubits") = QVec())

        .def("set_readout_error", [](NoiseQVM &self, const std::vector<std::vector<double>> &probs_list, const QVec &qubits){
                self.set_readout_error(probs_list, qubits);
            },
            py::arg("probs_list"), py::arg("qubits") = QVec())

        .def("set_rotation_error", [](NoiseQVM &self, double error){
                self.set_rotation_error(error);
            })

        /*will delete*/
        .def("initQVM", [](NoiseQVM& qvm, py::dict param)
            {
                py::object json = py::module::import("json");
                py::object dumps = json.attr("dumps");
                auto json_string = std::string(py::str(dumps(param)));
                rapidjson::Document doc(rapidjson::kObjectType);
                doc.Parse(json_string.c_str());
                qvm.init(doc); 
            }, "init quantum virtual machine")

        /* new interface */
        .def("init_qvm", [](NoiseQVM& qvm, py::dict param)
            {
                py::object json = py::module::import("json");
                py::object dumps = json.attr("dumps");
                auto json_string = std::string(py::str(dumps(param)));
                rapidjson::Document doc(rapidjson::kObjectType);
                doc.Parse(json_string.c_str());
                qvm.init(doc);
            }, "init quantum virtual machine")
		.def("directly_run", &NoiseQVM::directlyRun,
			"directly_run a prog", "program"_a, py::return_value_policy::reference)
        .def("init_state", [](NoiseQVM& self, const QStat & state, const QVec &qlist){
                self.initState(state, qlist);
            }, py::arg("state") = QStat(), py::arg("qlist") = QVec()
            ,py::return_value_policy::reference)

        .def("run_with_configuration", [](NoiseQVM &qvm, QProg & prog, vector<ClassicalCondition> & cc_vector, py::dict param)
            {
                py::object json = py::module::import("json");
                py::object dumps = json.attr("dumps");
                auto json_string = std::string(py::str(dumps(param)));
                rapidjson::Document doc;
                auto & alloc = doc.GetAllocator();
                doc.Parse(json_string.c_str());
                return qvm.runWithConfiguration(prog, cc_vector, doc);
            }, "program"_a, "cbit_list"_a, "data"_a, py::return_value_policy::automatic)
        .def("run_with_configuration", [](NoiseQVM &qvm, QProg & prog, vector<ClassicalCondition> & cc_vector, int shots)
            {return qvm.runWithConfiguration(prog, cc_vector, shots); }, "program"_a, "cbit_list"_a, "data"_a, py::return_value_policy::automatic)
        .def("run_with_configuration", [](QuantumMachine& qvm, QProg& prog, vector<int>& cbit_addrs, int shots)
            {return qvm.runWithConfiguration(prog, cbit_addrs, shots); }, "program"_a, "cbit_addr_list"_a, "data"_a, py::return_value_policy::automatic);
        
        py::class_<SingleAmplitudeQVM, QuantumMachine>(m, "SingleAmpQVM")
            .def(py::init<>())

            .def("init_qvm", &SingleAmplitudeQVM::init, "init quantum virtual machine")
            .def("run", [](SingleAmplitudeQVM &qvm, QProg &prog, QVec &qv, size_t max_rank, size_t alloted_time ){
                    return qvm.run(prog, qv, max_rank, alloted_time);
            }, py::arg("prog"), py::arg("qv"), py::arg("max_rank")= 30, py::arg("alloted_time") = 5, "run the quantum program", py::return_value_policy::automatic_reference)
			.def("run", [](SingleAmplitudeQVM& qvm, QProg &prog, QVec& qv, size_t max_rank, const std::vector<qprog_sequence_t>& sequences) {return qvm.run(prog, qv, max_rank, sequences); }, "run the quantum program")
			.def("get_sequence", &SingleAmplitudeQVM::getSequence, "get prog sequence", py::return_value_policy::automatic_reference)
			.def("get_quick_map_vertice", &SingleAmplitudeQVM::getQuickMapVertice, "get quick map vertice", py::return_value_policy::automatic_reference)

			.def("pmeasure_bin_index", &SingleAmplitudeQVM::pMeasureBinindex, "PMeasure by binary index", py::return_value_policy::automatic_reference)
			.def("pmeasure_dec_index", &SingleAmplitudeQVM::pMeasureDecindex, "PMeasure by decimal  index", py::return_value_policy::automatic_reference)

            .def("get_prob_dict", [](SingleAmplitudeQVM& self, QVec &qvec) {return self.getProbDict(qvec); })
            .def("prob_run_dict", [](SingleAmplitudeQVM& self, QProg prog, QVec &qvec) {return self.probRunDict(prog, qvec); })
            .def("get_prob_dict", [](SingleAmplitudeQVM& self, const std::vector<int>& qaddrs_vect) {return self.getProbDict(qaddrs_vect); })
			.def("prob_run_dict", [](SingleAmplitudeQVM& self, QProg prog, const std::vector<int>& qaddrs_vect) {return self.probRunDict(prog, qaddrs_vect); });

           
        py::enum_<BackendType>(m, "BackendType")
            .value("CPU", BackendType::CPU)
            .value("GPU", BackendType::GPU)
            .value("CPU_SINGLE_THREAD", BackendType::CPU_SINGLE_THREAD)
            .value("NOISE", BackendType::NOISE)
            .value("MPS", BackendType::MPS)
            .export_values();

        py::class_<PartialAmplitudeQVM, QuantumMachine>(m, "PartialAmpQVM")
            .def(py::init<>())

            .def("init_qvm", [](PartialAmplitudeQVM &qvm, BackendType type) {return qvm.init(type); },
                 py::arg("type") = BackendType::CPU, "init quantum virtual machine")

            .def("run", [](PartialAmplitudeQVM &qvm, QProg prog) {return qvm.run(prog); }, "load the quantum program")
            .def("run", [](PartialAmplitudeQVM &qvm, QCircuit cir) {return qvm.run(cir); }, "load the quantum program")

            .def("pmeasure_bin_index", &PartialAmplitudeQVM::pmeasure_bin_index, "bin_index"_a, "PMeasure_bin_index", py::return_value_policy::automatic_reference)
            .def("pmeasure_dec_index", &PartialAmplitudeQVM::pmeasure_dec_index, "dec_index"_a, "PMeasure_dec_index", py::return_value_policy::automatic_reference)
            .def("pmeasure_subset", &PartialAmplitudeQVM::pmeasure_subset, "index_list"_a, "pmeasure_subset", py::return_value_policy::automatic_reference)
            .def("get_prob_dict", [](PartialAmplitudeQVM &self, const QVec &qvec) {
                return self.getProbDict(qvec); })
            .def("prob_run_dict", [](PartialAmplitudeQVM &self, QProg prog, const QVec &qvec) {
                return self.probRunDict(prog, qvec); });

        py::class_<MPSQVM, QuantumMachine>(m, "MPSQVM")
            .def(py::init<>())

            .def("get_qstate", &MPSQVM::getQState, "getState", py::return_value_policy::automatic)
            .def("init_qvm", &MPSQVM::init, "init quantum virtual machine")

            .def("init_state", [](MPSQVM& self, const QStat & state, const QVec &qlist){
                    self.initState(state, qlist);
                }, py::arg("state") = QStat(), py::arg("qlist") = QVec()
                ,py::return_value_policy::reference)

            .def("get_allocate_qubit_num", get_allocate_qubit, "getAllocateQubitNum", py::return_value_policy::reference)
            .def("get_allocate_cmem_num", get_allocate_CMem, "getAllocateCMem", py::return_value_policy::reference)
			.def("directly_run", &MPSQVM::directlyRun,
			"directly_run a prog", "program"_a, py::return_value_policy::reference)
            .def("run_with_configuration", [](MPSQVM &qvm, QProg &prog, std::vector<ClassicalCondition> & cc_vector, py::dict param)
                {
                    py::object json = py::module::import("json"); 
                    py::object dumps = json.attr("dumps"); 
                    auto json_string = std::string(py::str(dumps(param))); 
                    rapidjson::Document doc; 
                    auto & alloc = doc.GetAllocator(); 
                    doc.Parse(json_string.c_str()); 
                    return qvm.runWithConfiguration(prog, cc_vector, doc); 
                }, "program"_a, "cbit_list"_a, "data"_a, py::return_value_policy::automatic)
                    .def("run_with_configuration", [](MPSQVM &qvm, QProg & prog, std::vector<ClassicalCondition> & cc_vector, int shots)
                { return qvm.runWithConfiguration(prog, cc_vector, shots); }, "program"_a, "cbit_list"_a, "data"_a, py::return_value_policy::automatic)
                    .def("pmeasure", &MPSQVM::PMeasure, "qubit_list"_a, "select_max"_a = -1, "Get the probability distribution over qubits", py::return_value_policy::reference)
                    .def("pmeasure_no_index", &MPSQVM::PMeasure_no_index, "qubit_list"_a, "Get the probability distribution over qubits", py::return_value_policy::reference)
                    .def("get_prob_tuple_list", &MPSQVM::getProbTupleList, "qubit_list"_a, "select_max"_a = -1, py::return_value_policy::reference)
                    .def("get_prob_list", &MPSQVM::getProbList, "qubit_list"_a, "select_max"_a = -1, py::return_value_policy::reference)
                    .def("get_prob_dict", &MPSQVM::getProbDict, "qubit_list"_a, "select_max"_a = -1, py::return_value_policy::reference)
                    .def("prob_run_tuple_list", &MPSQVM::probRunTupleList, "program"_a, "qubit_list"_a, "select_max"_a = -1, py::return_value_policy::reference)
                    .def("prob_run_list", &MPSQVM::probRunList, "program"_a, "qubit_list"_a, "select_max"_a = -1, py::return_value_policy::reference)
                    .def("prob_run_dict", &MPSQVM::probRunDict, "program"_a, "qubit_list"_a, "select_max"_a = -1, py::return_value_policy::reference)
                    .def("quick_measure", &MPSQVM::quickMeasure, "qubit_list"_a, "shots"_a, py::return_value_policy::reference)

                    .def("pmeasure_bin_index", &MPSQVM::pmeasure_bin_index, "program"_a, "string"_a, py::return_value_policy::reference)
                    .def("pmeasure_dec_index", &MPSQVM::pmeasure_dec_index, "program"_a, "string"_a, py::return_value_policy::reference)
                    .def("pmeasure_bin_subset", &MPSQVM::pmeasure_bin_subset, "program"_a, "string_list"_a, py::return_value_policy::reference)
                    .def("pmeasure_dec_subset", &MPSQVM::pmeasure_dec_subset, "program"_a, "string_list"_a, py::return_value_policy::reference)

                    //The all next MPSQVM functions are only for noise simulation

                    /* bit-flip,phase-flip,bit-phase-flip,phase-damping,amplitude-damping,depolarizing*/
                    .def("set_noise_model", [](MPSQVM &qvm, NOISE_MODEL model, GateType gate_type, double param)
                {return qvm.set_noise_model(model, gate_type, param); })
                    .def("set_noise_model", [](MPSQVM &qvm, NOISE_MODEL model, GateType gate_type, double param, const std::vector<QVec>& qvecs)
                {return qvm.set_noise_model(model, gate_type, param, qvecs); })

                    /*decoherence error*/
                    .def("set_noise_model", [](MPSQVM &qvm, NOISE_MODEL model, GateType gate_type, double T1, double T2, double time_param)
                {return qvm.set_noise_model(model, gate_type, T1, T2, time_param); })
                    .def("set_noise_model", [](MPSQVM &qvm, NOISE_MODEL model, GateType gate_type, double T1, double T2, double time_param, const std::vector<QVec>& qvecs)
                {return qvm.set_noise_model(model, gate_type, T1, T2, time_param, qvecs); })

                    /*mixed unitary error*/
                    .def("set_mixed_unitary_error", [](MPSQVM &qvm, GateType gate_type, const std::vector<QStat>& karus_matrices, const std::vector<QVec>& qubits_vecs)
                {return qvm.set_mixed_unitary_error(gate_type, karus_matrices, qubits_vecs); })
                    .def("set_mixed_unitary_error", [](MPSQVM &qvm, GateType gate_type, const std::vector<QStat>& unitary_matrices, prob_vec probs_vec, const std::vector<QVec>& qubits_vecs)
                {return qvm.set_mixed_unitary_error(gate_type, unitary_matrices, probs_vec, qubits_vecs); })
                    .def("set_mixed_unitary_error", [](MPSQVM &qvm, GateType gate_type, const std::vector<QStat>& karus_matrices)
                {return qvm.set_mixed_unitary_error(gate_type, karus_matrices); })
                    .def("set_mixed_unitary_error", [](MPSQVM &qvm, GateType gate_type, const std::vector<QStat>& unitary_matrices, prob_vec probs_vec)
                {return qvm.set_mixed_unitary_error(gate_type, unitary_matrices, probs_vec); })

                    /*readout error*/
                    .def("set_readout_error", &MPSQVM::set_readout_error, "readout_params"_a, "qubits"_a, py::return_value_policy::reference)

                    /*measurement error*/
                    .def("set_measure_error", [](MPSQVM &qvm, NOISE_MODEL model, double param) {return qvm.set_measure_error(model, param); })
                    .def("set_measure_error", [](MPSQVM &qvm, NOISE_MODEL model, double T1, double T2, double time_param) {return qvm.set_measure_error(model, T1, T2, time_param); })

                    /*rotation error*/
                    .def("set_rotation_error", &MPSQVM::set_rotation_error, "param"_a, py::return_value_policy::reference)

                    /*reset error*/
                    .def("set_reset_error", &MPSQVM::set_reset_error, "reset_0_param"_a, "reset_1_param"_a, py::return_value_policy::reference)

                    /* bit-flip,phase-flip,bit-phase-flip,phase-damping,amplitude-damping,depolarizing*/
                    .def("add_single_noise_model", [](MPSQVM &qvm, NOISE_MODEL model, GateType gate_type, double param)
                        {return qvm.add_single_noise_model(model, gate_type, param); })

                    /*decoherence error*/
                    .def("add_single_noise_model", [](MPSQVM &qvm, NOISE_MODEL model, GateType gate_type, double T1, double T2, double time_param)
                        {return qvm.add_single_noise_model(model, gate_type, T1, T2, time_param); });

            /*combine error*/
               
#ifdef USE_CURL

        py::enum_<RealChipType>(m, "real_chip_type")
            .value("origin_wuyuan_d3", RealChipType::ORIGIN_WUYUAN_D3)
            .value("origin_wuyuan_d4", RealChipType::ORIGIN_WUYUAN_D4)
            .value("origin_wuyuan_d5", RealChipType::ORIGIN_WUYUAN_D5)
            .export_values();

        py::enum_<TaskStatus>(m, "task_status")
            .value("waiting", TaskStatus::WAITING)
            .value("computing", TaskStatus::COMPUTING)
            .value("finished", TaskStatus::FINISHED)
            .value("failed", TaskStatus::FAILED)
            .value("queuing", TaskStatus::QUEUING)
            .export_values();

        //py::enum_<RealChipType>(m, "RealChipType")
        //    .value("origin_wuyuan_d3", RealChipType::ORIGIN_WUYUAN_D3)
        //    .value("origin_wuyuan_d4", RealChipType::ORIGIN_WUYUAN_D4)
        //    .value("origin_wuyuan_d5", RealChipType::ORIGIN_WUYUAN_D5)
        //    .export_values();

        //py::enum_<TaskStatus>(m, "TaskStatus")
        //    .value("waiting", TaskStatus::WAITING)
        //    .value("computing", TaskStatus::COMPUTING)
        //    .value("finished", TaskStatus::FINISHED)
        //    .value("failed", TaskStatus::FAILED)
        //    .value("queuing", TaskStatus::QUEUING)
        //    .export_values();

        py::class_<QCloudMachine, QuantumMachine>(m, "QCloud")
            .def(py::init<>())
            .def("init_qvm", [](QCloudMachine &qvm, std::string token, bool is_logged) {return qvm.init(token, is_logged); },
                py::arg("token"), py::arg("is_logged") = false, "init quantum virtual machine")

            //url setting
            .def("set_qcloud_api", [](QCloudMachine &qcm, std::string url) {return qcm.set_qcloud_api(url); })

            .def("set_inquire_url", [](QCloudMachine &qcm, std::string url) {return qcm.set_inquire_url(url); })
            .def("set_compute_url", [](QCloudMachine &qcm, std::string url) {return qcm.set_compute_url(url); })
            .def("set_batch_inquire_url", [](QCloudMachine &qcm, std::string url) {return qcm.set_batch_inquire_url(url); })
            .def("set_batch_compute_url", [](QCloudMachine &qcm, std::string url) {return qcm.set_batch_compute_url(url); })

            // noise
            .def("set_noise_model", [](QCloudMachine &qcm, NOISE_MODEL model, const std::vector<double> single_params, const std::vector<double> double_params) {return qcm.set_noise_model(model, single_params, double_params); })
            .def("noise_measure", [](QCloudMachine &qcm, 
                QProg &prog, 
                int shot,
                std::string task_name)

                {return qcm.noise_measure(prog, shot, task_name); },
                py::arg("prog"),
                py::arg("shot"),
                py::arg("task_name") = "Qurator Experiment")


            // full_amplitude
            .def("full_amplitude_measure", [](QCloudMachine &qcm, 
                QProg &prog, 
                int shot,
                std::string task_name)
        
                {return qcm.full_amplitude_measure(prog, shot, task_name); },
                py::arg("prog"),
                py::arg("shot"),
                py::arg("task_name") = "Qurator Experiment")


            .def("full_amplitude_pmeasure", [](QCloudMachine &qcm, 
                QProg &prog, 
                Qnum qvec,
                std::string task_name)
                
                {return qcm.full_amplitude_pmeasure(prog, qvec, task_name); },
                py::arg("prog"),
                py::arg("qvec"),
                py::arg("task_name") = "Qurator Experiment")


            //partial_amplitude
            .def("partial_amplitude_pmeasure", [](QCloudMachine &qcm, 
                QProg &prog, 
                std::vector<std::string> amp_vec, 
                std::string task_name) 
                
                {return qcm.partial_amplitude_pmeasure(prog, amp_vec, task_name); },
                py::arg("prog"),
                py::arg("amp_vec"),
                py::arg("task_name") = "Qurator Experiment")


            //single_amplitude
            .def("single_amplitude_pmeasure", [](QCloudMachine &qcm, 
                QProg &prog, 
                std::string amplitude,
                std::string task_name)
                
                {return qcm.single_amplitude_pmeasure(prog, amplitude, task_name); },
                py::arg("prog"),
                py::arg("amplitude"),
                py::arg("task_name") = "Qurator Experiment")


            //real chip measure
            .def("real_chip_measure", [](QCloudMachine &qcm,
                QProg &prog,
                int shot,
                RealChipType chip_id,
                bool is_amend,
                bool is_mapping,
                bool is_optimization,
                std::string task_name)

                {return qcm.real_chip_measure(prog, shot, chip_id, is_amend, is_mapping, is_optimization, task_name); },
                py::arg("prog"),
                py::arg("shot"),
                py::arg("chip_id") = RealChipType::ORIGIN_WUYUAN_D5,
                py::arg("is_amend") = true,
                py::arg("is_mapping") = true,
                py::arg("is_optimization") = true,
                py::arg("task_name") = "Qurator Experiment")


            //real chip get_state_fidelity
            .def("get_state_fidelity", [](QCloudMachine &qcm,
                QProg &prog,
                int shot,
                RealChipType chip_id,
                bool is_amend,
                bool is_mapping,
                bool is_optimization,
                std::string task_name)

                {return qcm.get_state_fidelity(prog, shot, chip_id, is_amend, is_mapping, is_optimization, task_name); },
                py::arg("prog"),
                py::arg("shot"),
                py::arg("chip_id") = RealChipType::ORIGIN_WUYUAN_D5,
                py::arg("is_amend") = true,
                py::arg("is_mapping") = true,
                py::arg("is_optimization") = true,
                py::arg("task_name") = "Qurator Experiment")


            //real chip get_state_tomography_density
            .def("get_state_tomography_density", [](QCloudMachine &qcm,
                QProg &prog,
                int shot,
                RealChipType chip_id,
                bool is_amend,
                bool is_mapping,
                bool is_optimization,
                std::string task_name)

                {return qcm.get_state_tomography_density(prog, shot, chip_id, is_amend, is_mapping, is_optimization, task_name); },
                py::arg("prog"),
                py::arg("shot"),
                py::arg("chip_id") = RealChipType::ORIGIN_WUYUAN_D5,
                py::arg("is_amend") = true,
                py::arg("is_mapping") = true,
                py::arg("is_optimization") = true,
                py::arg("task_name") = "Qurator Experiment")


            //get_expectation
            .def("get_expectation", [](QCloudMachine &qcm,
                QProg &prog,
                const QHamiltonian& hamiltonian,
                const QVec& qvec,
                TaskStatus& status,
                std::string task_name)

                {return qcm.get_expectation(prog, hamiltonian, qvec, status, task_name); },
                py::arg("prog"),
                py::arg("hamiltonian"),
                py::arg("qvec"),
                py::arg("status"),
                py::arg("task_name") = "Qurator Experiment")


            //get_expectation commit
            .def("get_expectation_commit", [](QCloudMachine &qcm,
                QProg &prog,
                const QHamiltonian& hamiltonian,
                const QVec& qvec,
                TaskStatus& status)

                {return qcm.get_expectation(prog, hamiltonian, qvec, status); },
                py::arg("prog"),
                py::arg("hamiltonian"),
                py::arg("qvec"),
                py::arg("status"))

            //get_expectation_exec
            .def("get_expectation_exec", [](QCloudMachine &qcm,
                std::string taskid,
                TaskStatus& status)

                {return qcm.get_expectation_exec(taskid, status); },
                py::arg("taskid"),
                py::arg("status"))

            //get_expectation_query
            .def("get_expectation_query", [](QCloudMachine &qcm,
                std::string taskid,
                TaskStatus& status)

                {return qcm.get_expectation_query(taskid, status); },
                py::arg("taskid"),
                py::arg("status"))

            // full_amplitude
            .def("full_amplitude_measure_batch", [](QCloudMachine &qcm,
                std::vector<QProg>& prog_array,
                int shot,
                std::string task_name)

                {return qcm.full_amplitude_measure_batch(prog_array, shot, task_name); },
                py::arg("prog_array"),
                py::arg("shot"),
                py::arg("task_name") = "QPanda Experiment")

            .def("full_amplitude_pmeasure_batch", [](QCloudMachine &qcm,
                std::vector<QProg>& prog_array,
                Qnum qvec,
                std::string task_name)

                {return qcm.full_amplitude_pmeasure_batch(prog_array, qvec, task_name); },
                py::arg("prog_array"),
                py::arg("qvec"),
                py::arg("task_name") = "QPanda Experiment")

            //partial_amplitude
            .def("partial_amplitude_pmeasure_batch", [](QCloudMachine &qcm,
                std::vector<QProg>& prog_array,
                std::vector<std::string> amp_vec,
                std::string task_name)

                {return qcm.partial_amplitude_pmeasure_batch(prog_array, amp_vec, task_name); },
                py::arg("prog_array"),
                py::arg("amp_vec"),
                py::arg("task_name") = "QPanda Experiment")

                //single_amplitude
                .def("single_amplitude_pmeasure_batch", [](QCloudMachine &qcm,
                    std::vector<QProg>& prog_array,
                    std::string amplitude,
                    std::string task_name)

                {return qcm.single_amplitude_pmeasure_batch(prog_array, amplitude, task_name); },
                py::arg("prog"),
                py::arg("amplitude"),
                py::arg("task_name") = "QPanda Experiment")

                //noise
                .def("noise_measure_batch", [](QCloudMachine &qcm,
                    std::vector<QProg>& prog_array,
                    int shot,
                    std::string task_name)

                    {return qcm.noise_measure_batch(prog_array, shot, task_name); },
                    py::arg("prog_array"),
                    py::arg("shot"),
                    py::arg("task_name") = "QPanda Experiment")

                //real chip measure
                .def("real_chip_measure_batch", [](QCloudMachine &qcm,
                    std::vector<QProg>& prog_array,
                    int shot,
                    RealChipType chip_id,
                    bool is_amend,
                    bool is_mapping,
                    bool is_optimization,
                    std::string task_name)

                    {return qcm.real_chip_measure_batch(prog_array, shot, chip_id, is_amend, is_mapping, is_optimization, task_name); },
                    py::arg("prog_array"),
                    py::arg("shot"),
                    py::arg("chip_id") = RealChipType::ORIGIN_WUYUAN_D3,
                    py::arg("is_amend") = true,
                    py::arg("is_mapping") = true,
                    py::arg("is_optimization") = true,
                    py::arg("task_name") = "QPanda Experiment")

                .def("full_amplitude_measure_batch_commit", [](QCloudMachine &qcm,
                    std::vector<QProg>& prog_array,
                    int shot,
                    TaskStatus& status,
                    std::string task_name)

                    {return qcm.full_amplitude_measure_batch_commit(prog_array, shot, status, task_name); },
                    py::arg("prog_array"),
                    py::arg("shot"),
                    py::arg("status"),
                    py::arg("task_name") = "QPanda Experiment")

                .def("full_amplitude_pmeasure_batch_commit", [](QCloudMachine &qcm,
                    std::vector<QProg>& prog_array,
                    Qnum qvec,
                    TaskStatus& status,
                    std::string task_name)

                    {return qcm.full_amplitude_pmeasure_batch_commit(prog_array, qvec, status, task_name); },
                    py::arg("prog_array"),
                    py::arg("qvec"),
                    py::arg("status"),
                    py::arg("task_name") = "QPanda Experiment")

                .def("real_chip_measure_batch_commit", [](QCloudMachine &qcm,
                    std::vector<QProg>& prog_array,
                    int shot,
                    TaskStatus& status,
                    RealChipType chip_id,
                    bool is_amend,
                    bool is_mapping,
                    bool is_optimization,
                    std::string task_name)

                    {return qcm.real_chip_measure_batch_commit(prog_array, shot, status, chip_id, is_amend, is_mapping, is_optimization, task_name); },
                    py::arg("prog_array"),
                    py::arg("shot"),
                    py::arg("status"),
                    py::arg("chip_id") = RealChipType::ORIGIN_WUYUAN_D3,
                    py::arg("is_amend") = true,
                    py::arg("is_mapping") = true,
                    py::arg("is_optimization") = true,
                    py::arg("task_name") = "QPanda Experiment")

                .def("full_amplitude_measure_batch_query", [](QCloudMachine &qcm,
                    std::map<size_t, std::string> taskid_map)

                    {return qcm.full_amplitude_measure_batch_query(taskid_map); },
                    py::arg("taskid_map"))

                .def("full_amplitude_pmeasure_batch_query", [](QCloudMachine &qcm,
                    std::map<size_t, std::string> taskid_map)

                    {return qcm.full_amplitude_pmeasure_batch_query(taskid_map); },
                    py::arg("taskid_map"))

                .def("real_chip_measure_batch_query", [](QCloudMachine &qcm,
                    std::map<size_t, std::string> taskid_map)

                    {return qcm.real_chip_measure_batch_query(taskid_map); },
                    py::arg("taskid_map"));


#endif // USE_CURL

        py::implicitly_convertible<Qubit *, QVec>();
        py::implicitly_convertible<std::vector<Qubit *>, QVec>();
        py::implicitly_convertible<CPUQVM, QuantumMachine>();
        py::implicitly_convertible<GPUQVM, QuantumMachine>();
        py::implicitly_convertible<CPUSingleThreadQVM, QuantumMachine>();
        py::implicitly_convertible<NoiseQVM, QuantumMachine>();
        py::implicitly_convertible<SingleAmplitudeQVM, QuantumMachine>();
        py::implicitly_convertible<PartialAmplitudeQVM, QuantumMachine>();
}
