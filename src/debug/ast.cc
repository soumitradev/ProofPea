#include <debug/ast.h>

namespace debug {
namespace ast {

GVC_t *graphvizInit() {
  // TODO: Track errors in this function
  GVC_t *ctx = gvContext();
  const char *astFilename = "ast.svg";

  // Create the AST output file
  std::ofstream astFile(astFilename);
  astFile.close();

  // Pass args to graphviz
  const char *args[] = {"dot", "-Tsvg", "-o", (char *)astFilename};
  gvParseArgs(ctx, 4, (char **)args);

  return ctx;
}

void graphvizClose(GVC_t *ctx, Agraph_t *graph) {
  // Compute, render and write the graph according to args passed to
  // graphvizContext
  // TODO: Track errors in this function
  gvLayoutJobs(ctx, graph);
  gvRenderJobs(ctx, graph);

  // Deallocate data
  gvFreeLayout(ctx, graph);
  agclose(graph);

  gvFreeContext(ctx);
}

Agnode_t *renderNode(Agraph_t *graph, void *nodePtr, const std::string lexeme,
                     parser::NodeType type) {
  // TODO: Track errors in this function
  char *name = nullptr;
  if (type != parser::ATOM) {
    // Use the pointer to the operator to make a unique name for the operator
    std::ostringstream pointerStream;
    pointerStream << nodePtr;
    name = pointerStream.str().data();
  } else {
    // Use the lexeme of the atom itself, since atoms with the same lexeme are
    // the same
    name = (char *)lexeme.data();
  }

  Agnode_t *renderedNode = agnode(graph, name, 1);

  // Set the display label to the lexeme of the node
  if (lexeme != "->") {
    agsafeset(renderedNode, (char *)"label", (char *)lexeme.data(), (char *)"");
  } else {
    // Set a nice readable symbol for implies
    agsafeset(renderedNode, (char *)"label", (char *)"⇒", (char *)"");
  }

  // Render atoms in red
  if (type == parser::ATOM) {
    agsafeset(renderedNode, (char *)"color", (char *)"red", (char *)"");
  }

  return renderedNode;
}

void renderASTRecursive(const Agraph_t *graph, const parser::Node *node,
                        const Agnode_t *parent) {
  // TODO: Track errors in this function
  Agnode_t *renderedNode = nullptr;
  if (node->type == parser::UNARY) {
    const auto unaryOperatorNode =
        std::get<const parser::UnaryOperator *>(node->node);

    renderedNode = renderNode((Agraph_t *)graph, (void *)unaryOperatorNode,
                              unaryOperatorNode->op->lexeme, node->type);
    renderASTRecursive(graph, unaryOperatorNode->child, renderedNode);
  } else if (node->type == parser::BINARY) {
    const auto binaryOperatorNode =
        std::get<const parser::BinaryOperator *>(node->node);
    renderedNode = renderNode((Agraph_t *)graph, (void *)binaryOperatorNode,
                              binaryOperatorNode->op->lexeme, node->type);
    renderASTRecursive(graph, binaryOperatorNode->left, renderedNode);
    renderASTRecursive(graph, binaryOperatorNode->right, renderedNode);
  } else if (node->type == parser::ATOM) {
    const auto atomNode = std::get<const parser::Atom *>(node->node);
    renderedNode = renderNode((Agraph_t *)graph, (void *)atomNode,
                              atomNode->token->lexeme, node->type);
  }

  if (parent == nullptr) return;
  agedge((Agraph_t *)graph, (Agnode_t *)parent, renderedNode, 0, 1);
}

void printAST(const parser::AST *ast) {
  // TODO: Track errors in this function
  const auto ctx = graphvizInit();

  Agraph_t *astGraph = agopen((char *)"g", Agdirected, 0);

  renderASTRecursive(astGraph, ast->root, nullptr);

  graphvizClose(ctx, astGraph);
}
}  // namespace ast
}  // namespace debug
